#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the Qt-DAB program
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"ofdm-processor.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"radio.h"
#include	"dab-params.h"
#include	"fft.h"
//
#define	SEARCH_RANGE		(2 * 36)

/**
  *	\brief ofdmProcessor
  *	The ofdmProcessor class is the driver of the processing
  *	of the samplestream.
  *	It takes as parameter (a.o) the handler for the
  *	input device as well as the interpreters for
  *	FIC blocks and for MSC blocks.
  *	Local is a class ofdmDecoder that will - as the name suggests -
  *	map samples to bits and that will pass on the bits
  *	to the interpreters for FIC and MSC
  */

static	inline
int16_t	valueFor (int16_t b) {
int16_t	res	= 1;
	while (--b > 0)
	   res <<= 1;
	return res;
}

	ofdmProcessor::ofdmProcessor	(RadioInterface	*mr,
	                                 virtualInput	*theRig,
	                                 uint8_t	dabMode,
	                                 mscHandler 	*msc,
	                                 ficHandler 	*fic,
	                                 int16_t	threshold
#ifdef	HAVE_SPECTRUM
		                        ,RingBuffer<DSPCOMPLEX>	*spectrumBuffer,
	                                 RingBuffer<DSPCOMPLEX>	*iqBuffer
#endif
	                                 ):
	                                   params (dabMode),
	                                   phaseSynchronizer (dabMode, 
                                                              threshold),
#if defined (TII_ATTEMPT) || defined (TII_COORDINATES)
	                                   my_TII_Detector (dabMode), 
#endif
	                                   my_ofdmDecoder (mr,
	                                                   dabMode,
#ifdef	HAVE_SPECTRUM
	                                                   iqBuffer,
#endif
	                                                   fic,
	                                                   msc) {
int32_t	i;

	this	-> myRadioInterface	= mr;
	this	-> theRig		= theRig;
	this	-> my_ficHandler	= fic;

	this	-> T_null		= params. get_T_null ();
	this	-> T_s			= params. get_T_s ();
	this	-> T_u			= params. get_T_u ();
	this	-> T_F			= params. get_T_F ();
	this	-> nrBlocks		= params. get_L ();
	this	-> carriers		= params. get_carriers ();
	this	-> carrierDiff		= params. get_carrierDiff ();
	fft_handler			= new common_fft (T_u);
	fft_buffer			= fft_handler -> getVector ();
	dumping				= false;
	dumpIndex			= 0;
	dumpScale			= valueFor (theRig -> bitDepth ());

#ifdef  HAVE_SPECTRUM
        bufferSize      = 32768;
        this    -> spectrumBuffer       = spectrumBuffer;
        connect (this, SIGNAL (showSpectrum (int)),
                 mr, SLOT (showSpectrum (int)));
        localBuffer     = new DSPCOMPLEX [bufferSize];
        localCounter    = 0;
#endif
//
#ifdef	TII_COORDINATES
	tiiCoordinates			= false;
	connect (this, SIGNAL (showCoordinates (float, float)),
	         mr,   SLOT   (showCoordinates (float, float)));
#endif
	ofdmBuffer			= new DSPCOMPLEX [2 * T_s];
	ofdmBufferIndex			= 0;
	ofdmSymbolCount			= 0;
	tokenCount			= 0;
	sampleCnt			= 0;
/**
  *	the class phaseReference will take a number of samples
  *	and indicate - using some threshold - whether there is
  *	a strong correlation or not.
  *	It is used to decide on the first non-null sample
  *	of the frame.
  *	The size of the blocks handed over for inspection
  *	is T_u
  */
/**
  *	the ofdmDecoder takes time domain samples, will do an FFT,
  *	map the result on (soft) bits and hand over control for handling
  *	the decoded blocks
  */
	fineCorrector		= 0;	
	coarseCorrector		= 0;
	f2Correction		= true;
	oscillatorTable		= new DSPCOMPLEX [INPUT_RATE];
	localPhase		= 0;
	attempts		= 0;
	scanMode		= false;

	for (i = 0; i < INPUT_RATE; i ++)
	   oscillatorTable [i] = DSPCOMPLEX (cos (2.0 * M_PI * i / INPUT_RATE),
	                                     sin (2.0 * M_PI * i / INPUT_RATE));

	connect (this, SIGNAL (show_fineCorrector (int)),
	         myRadioInterface, SLOT (set_fineCorrectorDisplay (int)));
	connect (this, SIGNAL (show_coarseCorrector (int)),
	         myRadioInterface, SLOT (set_coarseCorrectorDisplay (int)));
	connect (this, SIGNAL (setSynced (char)),
	         myRadioInterface, SLOT (setSynced (char)));
	connect (this, SIGNAL (No_Signal_Found (void)),
	         myRadioInterface, SLOT (No_Signal_Found(void)));
	connect (this, SIGNAL (setSyncLost (void)),
	         myRadioInterface, SLOT (setSyncLost (void)));

	bufferContent	= 0;
//
	start ();
}

	ofdmProcessor::~ofdmProcessor	(void) {
	running		= false;	// this will cause an
	                                // exception to be raised
	                        	// through the getSample(s) functions.
	msleep (100);
	if (isRunning ()) {
	   wait ();
	   my_ofdmDecoder. stop ();
	}
	delete		ofdmBuffer;
	delete		oscillatorTable;
	delete		fft_handler;
}


/**
  *	\brief getSample
  *	Profiling shows that gettting a sample, together
  *	with the frequency shift, is a real performance killer.
  *	we therefore distinguish between getting a single sample
  *	and getting a vector full of samples
  */

DSPCOMPLEX ofdmProcessor::getSample (int32_t phase) {
DSPCOMPLEX temp;
	if (!running)
	   throw 21;

///	bufferContent is an indicator for the value of ... -> Samples ()
	if (bufferContent == 0) {
	   bufferContent = theRig -> Samples ();
	   while ((bufferContent == 0) && running) {
	      usleep (10);
	      bufferContent = theRig -> Samples (); 
	   }
	}

	if (!running)	
	   throw 20;
//
//	so here, bufferContent > 0
	theRig -> getSamples (&temp, 1);
	bufferContent --;
	if (dumping) {
           dumpBuffer [2 * dumpIndex    ] = real (temp) * dumpScale;
           dumpBuffer [2 * dumpIndex + 1] = imag (temp) * dumpScale;
           if ( ++dumpIndex >= DUMPSIZE / 2) {
              sf_writef_short (dumpFile, dumpBuffer, dumpIndex);
              dumpIndex = 0;
           }
        }
#ifdef  HAVE_SPECTRUM
	if (localCounter < bufferSize)
	   localBuffer [localCounter ++]        = temp;
#endif
//
//	OK, we have a sample!!
//	first: adjust frequency. We need Hz accuracy
	localPhase	-= phase;
	localPhase	= (localPhase + INPUT_RATE) % INPUT_RATE;

	temp		*= oscillatorTable [localPhase];
	sLevel		= 0.00001 * jan_abs (temp) + (1 - 0.00001) * sLevel;
#define	N	5
	sampleCnt	++;
	if (++ sampleCnt > INPUT_RATE / N) {
	   show_fineCorrector	(fineCorrector);
	   show_coarseCorrector	(coarseCorrector / KHz (1));
	   sampleCnt = 0;
#ifdef  HAVE_SPECTRUM
           spectrumBuffer -> putDataIntoBuffer (localBuffer, localCounter);
           emit showSpectrum (bufferSize);
           localCounter = 0;
#endif
	}
	return temp;
}
//

void	ofdmProcessor::getSamples (DSPCOMPLEX *v, int16_t n, int32_t phase) {
int32_t		i;

	if (!running)
	   throw 21;
	if (n > bufferContent) {
	   bufferContent = theRig -> Samples ();
	   while ((bufferContent < n) && running) {
	      usleep (10);
	      bufferContent = theRig -> Samples ();
	   }
	}
	if (!running)	
	   throw 20;
//
//	so here, bufferContent >= n
	n	= theRig -> getSamples (v, n);
	bufferContent -= n;
	if (dumping) {
           for (i = 0; i < n; i ++) {
              dumpBuffer [2 * dumpIndex    ] = real (v [i]) * dumpScale;
              dumpBuffer [2 * dumpIndex + 1] = imag (v [i]) * dumpScale;
              if (++dumpIndex >= DUMPSIZE / 2) {
                 sf_writef_short (dumpFile, dumpBuffer, dumpIndex);
                 dumpIndex = 0;
              }
           }
        }

//	OK, we have samples!!
//	first: adjust frequency. We need Hz accuracy
	for (i = 0; i < n; i ++) {
	   localPhase	-= phase;
	   localPhase	= (localPhase + INPUT_RATE) % INPUT_RATE;
#ifdef  HAVE_SPECTRUM
           if (localCounter < bufferSize)
              localBuffer [localCounter ++]     = v [i];
#endif
	   v [i]	*= oscillatorTable [localPhase];
	   sLevel	= 0.00001 * jan_abs (v [i]) + (1 - 0.00001) * sLevel;
	}

	sampleCnt	+= n;
	if (sampleCnt > INPUT_RATE / N) {
	   show_fineCorrector	(fineCorrector);
	   show_coarseCorrector	(coarseCorrector / KHz (1));
#ifdef  HAVE_SPECTRUM
           spectrumBuffer -> putDataIntoBuffer (localBuffer, bufferSize);
           emit showSpectrum (bufferSize);
           localCounter = 0;
#endif
	   sampleCnt = 0;
	}
}
/***
   *	\brief run
   *	The main thread, reading samples,
   *	time synchronization and frequency synchronization
   *	Identifying blocks in the DAB frame
   *	and sending them to the ofdmDecoder who will transfer the results
   *	Finally, estimating the small freqency error
   */
void	ofdmProcessor::run	(void) {
int32_t		startIndex;
int32_t		i;
DSPCOMPLEX	FreqCorr;
int32_t		counter;
float		currentStrength;
int32_t		syncBufferIndex	= 0;
int32_t		syncBufferSize	= 32768;
int32_t		syncBufferMask	= syncBufferSize - 1;
float		envBuffer	[syncBufferSize];

	coarseCorrector = 0;
        fineCorrector   = 0;
        f2Correction    = true;
        syncBufferIndex = 0;
        sLevel          = 0;
        localPhase      = 0;
	attempts	= 0;
        theRig  -> resetBuffer ();
        running         = true;
	my_ofdmDecoder. start ();

	try {

	   sLevel	= 0;
	   for (i = 0; i < T_F / 5; i ++) {
	      jan_abs (getSample (0));
	   }
Initing:
notSynced:
	   syncBufferIndex	= 0;
	   currentStrength	= 0;
///	first, we need samples to get a reasonable sLevel

//	read in T_s samples for a next attempt;
	   syncBufferIndex = 0;
	   currentStrength	= 0;
	   for (i = 0; i < 50; i ++) {
	      DSPCOMPLEX sample			= getSample (0);
	      envBuffer [syncBufferIndex]	= jan_abs (sample);
	      currentStrength 			+= envBuffer [syncBufferIndex];
	      syncBufferIndex ++;
	   }
/**
  *	We now have initial values for currentStrength (i.e. the sum
  *	over the last 50 samples) and sLevel, the long term average.
  */
SyncOnNull:
/**
  *	here we start looking for the null level, i.e. a dip
  */
	   counter	= 0;
	   setSynced (false);
	   while (currentStrength / 50  > 0.50 * sLevel) {
	      DSPCOMPLEX sample	=
	                      getSample (coarseCorrector + fineCorrector);
	      envBuffer [syncBufferIndex] = jan_abs (sample);
//	update the levels
	      currentStrength += envBuffer [syncBufferIndex] -
	                         envBuffer [(syncBufferIndex - 50) & syncBufferMask];
	      syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
	      counter ++;
	      if (counter > T_F) { // hopeless
	         if (scanMode && (++attempts >= 5)) {
	            emit (No_Signal_Found ());
                    attempts = 0;
                 }

	         goto notSynced;
	      }
	   }
/**
  *	It seemed we found a dip that started app 65/100 * 50 samples earlier.
  *	We now start looking for the end of the null period.
  */
	   counter	= 0;
SyncOnEndNull:
	   while (currentStrength / 50 < 0.75 * sLevel) {
	      DSPCOMPLEX sample = getSample (coarseCorrector + fineCorrector);
	      envBuffer [syncBufferIndex] = jan_abs (sample);
//	update the levels
	      currentStrength += envBuffer [syncBufferIndex] -
	                         envBuffer [(syncBufferIndex - 50) & syncBufferMask];
	      syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
	      counter	++;
//
	      if (counter > T_null + 50) { // hopeless
	         goto notSynced;
	      }
	   }
/**
  *	The end of the null period is identified, probably about 40
  *	samples earlier.
  */
SyncOnPhase:
/**
  *	We now have to find the exact first sample of the non-null period.
  *	We use a correlation that will find the first sample after the
  *	cyclic prefix.
  *	When in "sync", i.e. pretty sure that we know were we are,
  *	we skip the "dip" identification and come here right away.
  *
  *	now read in Tu samples. The precise number is not really important
  *	as long as we can be sure that the first sample to be identified
  *	is part of the samples read.
  */
	getSamples (ofdmBuffer, T_u, coarseCorrector + fineCorrector);
//
//	and then, call upon the phase synchronizer to verify/compute
//	the real "first" sample
	   startIndex = phaseSynchronizer. findIndex (ofdmBuffer);
	   if (startIndex < 0) { // no sync, try again
	      if (f2Correction) {
	         setSyncLost ();
	      }
	      goto notSynced;
	   }
/**
  *	Once here, we are synchronized, we need to copy the data we
  *	used for synchronization for block 0
  */
	   memmove (ofdmBuffer, &ofdmBuffer [startIndex],
	                  (T_u - startIndex) * sizeof (DSPCOMPLEX));
	   ofdmBufferIndex	= T_u - startIndex;

Block_0:
/**
  *	Block 0 is special in that it is used for fine time synchronization,
  *	for coarse frequency synchronization
  *	and its content is used as a reference for decoding the
  *	first datablock.
  *	We read the missing samples in the ofdm buffer
  */
	   setSynced (true);
	   getSamples (&ofdmBuffer [ofdmBufferIndex],
	               T_u - ofdmBufferIndex,
	               coarseCorrector + fineCorrector);
	   my_ofdmDecoder. processBlock_0 (ofdmBuffer);
//
//	Here we look only at the block_0 when we need a coarse
//	frequency synchronization.
//	The width is limited to 2 * 35 Khz (i.e. positive and negative)
	   f2Correction	= !my_ficHandler -> syncReached ();
	   if (f2Correction) {
	      int correction		= processBlock_0 (ofdmBuffer);
	      if (correction != 100) {
	         coarseCorrector	+= correction * carrierDiff;
	         if (abs (coarseCorrector) > Khz (35))
	            coarseCorrector = 0;
	      }
	   }
/**
  *	after block 0, we will just read in the other (params -> L - 1) blocks
  */
Data_blocks:
/**
  *	The first ones are the FIC blocks. We immediately
  *	start with building up an average of the phase difference
  *	between the samples in the cyclic prefix and the
  *	corresponding samples in the datapart.
  */
	   FreqCorr		= DSPCOMPLEX (0, 0);
	   for (ofdmSymbolCount = 1;
	        ofdmSymbolCount < 4; ofdmSymbolCount ++) {
	      getSamples (ofdmBuffer, T_s, coarseCorrector + fineCorrector);
	      for (i = (int)T_u; i < (int)T_s; i ++) 
	         FreqCorr += ofdmBuffer [i] * conj (ofdmBuffer [i - T_u]);
	
	      my_ofdmDecoder. decodeFICblock (ofdmBuffer, ofdmSymbolCount);
	   }

///	and similar for the (params -> L - 4) MSC blocks
	   for (ofdmSymbolCount = 4;
	        ofdmSymbolCount <  (uint16_t)nrBlocks;
	        ofdmSymbolCount ++) {
	      getSamples (ofdmBuffer, T_s, coarseCorrector + fineCorrector);
	      for (i = (int32_t)T_u; i < (int32_t)T_s; i ++) 
	         FreqCorr += ofdmBuffer [i] * conj (ofdmBuffer [i - T_u]);

	      my_ofdmDecoder. decodeMscblock (ofdmBuffer, ofdmSymbolCount);
	   }

NewOffset:
///	we integrate the newly found frequency error with the
///	existing frequency error.
	   fineCorrector += 0.1 * arg (FreqCorr) / M_PI * (carrierDiff / 2);
//
/**
  *	OK,  here we are at the end of the frame
  *	Assume everything went well and skip T_null samples
  */
	   syncBufferIndex	= 0;
	   currentStrength	= 0;
	   getSamples (ofdmBuffer, T_null, coarseCorrector + fineCorrector);

#ifdef TII_COORDINATES
	   if (tiiCoordinates) {
	      int16_t mainId	= my_ficHandler -> mainId ();
	      if (mainId > 0) {
                 int16_t subId =  my_TII_Detector. find_C (ofdmBuffer,
	                                                   phaseSynchronizer. refTable,
	                                                   mainId); 
	         if (subId >= 0) {
	            bool found;
	            DSPCOMPLEX coord =
	                      my_ficHandler -> get_coordinates (mainId,
	                                                        subId,
	                                                        &found);
	            if (found) {
	               showCoordinates (real (coord), imag (coord));
	            }
	         }
	      }
	      tiiCoordinates = false;
	   }
#endif
	           
/**
  *	The first sample to be found for the next frame should be T_g
  *	samples ahead
  *	Here we just check the fineCorrector
  */
	   counter	= 0;
//

	   if (fineCorrector > carrierDiff / 2) {
	      coarseCorrector += carrierDiff;
	      fineCorrector -= carrierDiff;
	   }
	   else
	   if (fineCorrector < -carrierDiff / 2) {
	      coarseCorrector -= carrierDiff;
	      fineCorrector += carrierDiff;
	   }
ReadyForNewFrame:
///	and off we go, up to the next frame
	   goto SyncOnPhase;
	}
	catch (int e) {
	   ;
	}
	my_ofdmDecoder. stop ();
}

void	ofdmProcessor:: reset	(void) {
	running	= false;
	if (isRunning ()) {
//	   terminate ();
	   
	   wait ();
	}
	start ();
}

void	ofdmProcessor::stop	(void) {
	running	= false;
}

void	ofdmProcessor::startDumping	(SNDFILE *f) {
	if (dumping)
	   return;
//	do not change the order here.
	dumpFile 	= f;
	dumping		= true;
	dumpIndex	= 0;
}

void	ofdmProcessor::stopDumping	(void) {
	dumping = false;
}
//

void	ofdmProcessor::coarseCorrectorOn (void) {
	f2Correction 	= true;
	coarseCorrector	= 0;
}

void	ofdmProcessor::coarseCorrectorOff (void) {
	f2Correction	= false;
}

//
//	Processing block 0 here is needed as long as we are not in sync.
//	Several algorithms were tried, plain  correlating the
//	block 0, as used in locating the start index, with the
//	data block here did not work very well. The structure
//	of block 0 does not seem to lenf itself for that kind of matching
//	The current approach is to look for a particular pattern
//	of phiase differences between successive carriers.
int16_t	ofdmProcessor::processBlock_0 (DSPCOMPLEX *v) {
int16_t	i, j, index = 100;

	memcpy (fft_buffer, v, T_u * sizeof (DSPCOMPLEX));
	fft_handler	-> do_FFT ();

//	An alternative way is to look at a special pattern consisting
//	of zeros in the row of phasedifferences between successive carriers.
//	we investigate a sequence of phasedifferences that should
//	be zero around carrier 0
	float Mmin	= 1000;
	for (i = T_u - SEARCH_RANGE / 2; i < T_u + SEARCH_RANGE / 2; i ++) {
           float a1  =  abs (abs (arg (fft_buffer [(i + 1) % T_u] *
                          conj (fft_buffer [(i + 2) % T_u])) / M_PI) - 1);
           float a2  =  abs (abs (arg (fft_buffer [(i + 2) % T_u] *
                          conj (fft_buffer [(i + 3) % T_u])) / M_PI) - 1);
	   float a3  = abs (arg (fft_buffer [(i + 3) % T_u] *
	   	                    conj (fft_buffer [(i + 4) % T_u])));
	   float a4	= abs (arg (fft_buffer [(i + 4) % T_u] *
	   	                    conj (fft_buffer [(i + 5) % T_u])));
	   float a5	= abs (arg (fft_buffer [(i + 5) % T_u] *
	   	                    conj (fft_buffer [(i + 6) % T_u])));
	   float b1	= abs (abs (arg (fft_buffer [(i + 16 + 1) % T_u] *
	   	                    conj (fft_buffer [(i + 16 + 3) % T_u])) / M_PI) - 1);
	   float b2	= abs (arg (fft_buffer [(i + 16 + 3) % T_u] *
	   	                    conj (fft_buffer [(i + 16 + 4) % T_u])));
	   float b3	= abs (arg (fft_buffer [(i + 16 + 4) % T_u] *
	   	                    conj (fft_buffer [(i + 16 + 5) % T_u])));
	   float b4	= abs (arg (fft_buffer [(i + 16 + 5) % T_u] *
	   	                    conj (fft_buffer [(i + 16 + 6) % T_u])));
	   float sum = a1 + a2 + a3 + a4 + a5 + b1 + b2 + b3 + b4;
	   if (sum < Mmin) {
	      Mmin = sum;
	      index = i;
	   }
	}
	return index - T_u;
}

int16_t	ofdmProcessor::getMiddle (DSPCOMPLEX *v) {
int16_t		i;
DSPFLOAT	sum = 0;
int16_t		maxIndex = 0;
DSPFLOAT	oldMax	= 0;
//
//	basic sum over K carriers that are - most likely -
//	in the range
//	The range in which the carrier should be is
//	T_u / 2 - K / 2 .. T_u / 2 + K / 2
//	We first determine an initial sum over params -> K carriers
	for (i = 40; i < carriers + 40; i ++)
	   sum += abs (v [(T_u / 2 + i) % T_u]);
//
//	Now a moving sum, look for a maximum within a reasonable
//	range (around (T_u - K) / 2, the start of the useful frequencies)
	for (i = 40; i < T_u - (carriers - 40); i ++) {
	   sum -= abs (v [(T_u / 2 + i) % T_u]);
	   sum += abs (v [(T_u / 2 + i + carriers) % T_u]);
	   if (sum > oldMax) {
	      sum = oldMax;
	      maxIndex = i;
	   }
	}
	return maxIndex - (T_u - carriers) / 2;
}
	
void	ofdmProcessor::set_scanMode	(bool b) {
	scanMode	= b;
	attempts	= 0;
}

void	ofdmProcessor::set_tiiCoordinates	(void) {
	tiiCoordinates	= true;
}

