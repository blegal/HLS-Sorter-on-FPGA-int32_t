// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
/*******************************************************************************
Vendor: Xilinx
Associated Filename: bytestrm_dwordproc.cpp
Purpose:Vivado HLS Coding Style example
Device: All
Revision History: May 30, 2008 - initial release

*******************************************************************************
#-  (c) Copyright 2011-2019 Xilinx, Inc. All rights reserved.
#-
#-  This file contains confidential and proprietary information
#-  of Xilinx, Inc. and is protected under U.S. and
#-  international copyright and other intellectual property
#-  laws.
#-
#-  DISCLAIMER
#-  This disclaimer is not a license and does not grant any
#-  rights to the materials distributed herewith. Except as
#-  otherwise provided in a valid license issued to you by
#-  Xilinx, and to the maximum extent permitted by applicable
#-  law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
#-  WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
#-  AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
#-  BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
#-  INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
#-  (2) Xilinx shall not be liable (whether in contract or tort,
#-  including negligence, or under any other theory of
#-  liability) for any loss or damage of any kind or nature
#-  related to, arising under or in connection with these
#-  materials, including for any direct, or any indirect,
#-  special, incidental, or consequential loss or damage
#-  (including loss of data, profits, goodwill, or any type of
#-  loss or damage suffered as a result of any action brought
#-  by a third party) even if such damage or loss was
#-  reasonably foreseeable or Xilinx had been advised of the
#-  possibility of the same.
#-
#-  CRITICAL APPLICATIONS
#-  Xilinx products are not designed or intended to be fail-
#-  safe, or for use in any application requiring fail-safe
#-  performance, such as life-support or safety devices or
#-  systems, Class III medical devices, nuclear facilities,
#-  applications related to the deployment of airbags, or any
#-  other applications that could lead to death, personal
#-  injury, or severe property or environmental damage
#-  (individually and collectively, "Critical
#-  Applications"). Customer assumes the sole risk and
#-  liability of any use of Xilinx products in Critical
#-  Applications, subject only to applicable laws and
#-  regulations governing limitations on product liability.
#-
#-  THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
#-  PART OF THIS FILE AT ALL TIMES. 
#- ************************************************************************


This file contains confidential and proprietary information of Xilinx, Inc. and
is protected under U.S. and international copyright and other intellectual
property laws.

DISCLAIMER
This disclaimer is not a license and does not grant any rights to the materials
distributed herewith. Except as otherwise provided in a valid license issued to
you by Xilinx, and to the maximum extent permitted by applicable law:
(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX
HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR
FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether
in contract or tort, including negligence, or under any other theory of
liability) for any loss or damage of any kind or nature related to, arising under
or in connection with these materials, including for any direct, or any indirect,
special, incidental, or consequential loss or damage (including loss of data,
profits, goodwill, or any type of loss or damage suffered as a result of any
action brought by a third party) even if such damage or loss was reasonably
foreseeable or Xilinx had been advised of the possibility of the same.

CRITICAL APPLICATIONS
Xilinx products are not designed or intended to be fail-safe, or for use in any
application requiring fail-safe performance, such as life-support or safety
devices or systems, Class III medical devices, nuclear facilities, applications
related to the deployment of airbags, or any other applications that could lead
to death, personal injury, or severe property or environmental damage
(individually and collectively, "Critical Applications"). Customer asresultes the
sole risk and liability of any use of Xilinx products in Critical Applications,
subject only to applicable laws and regulations governing limitations on product
liability.

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT
ALL TIMES.

*******************************************************************************/

#include <stdint.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "bytestrm_dwordproc.h"

using namespace std;

using namespace hls;

//
//
//
////////////////////////////////////////////////////////////////////////////////
//
//
#define TYPE	int32_t
#define LENGTH	128
//
//
//
////////////////////////////////////////////////////////////////////////////////
//
//
template<class TYPE, int32_t MAX_SIZE>
void data_sorter(
		hls::stream<TYPE> &strm_in,
		hls::stream<TYPE> &strm_out
      )
{
	TYPE table[MAX_SIZE];

	for(uint8_t y = 0; y < MAX_SIZE; y += 1 ){
#pragma HLS PIPELINE
		table[y] = strm_in.read();
	}

	for(uint8_t y = 0; y < MAX_SIZE - 1; y += 1 )
	{
		TYPE last = table[0];
		for(short x = 1; x < MAX_SIZE; x += 1 )
		{
#pragma HLS PIPELINE
			const TYPE newv = table[x];
			if( last > newv )
			{
			    table[x-1] = newv;
			}else{
			    table[x-1] = last;
			    last       = newv;
			}
		}
	    table[MAX_SIZE-1] = last;
	}

	for(uint8_t y = 0; y < MAX_SIZE; y += 1 ){
#pragma HLS PIPELINE
        strm_out.write( table[y] );
	}

}
//
//
//
////////////////////////////////////////////////////////////////////////////////
//
//
// The top-level function
void stream_sorter(
		hls::stream<uint8_t> &strm_in,
		hls::stream<uint8_t> &strm_out
      )
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS interface axis port=strm_in
#pragma HLS interface axis port=strm_out

	stream<int32_t> dwordstrm_in;
	stream<int32_t> dwordstrm_out;
#pragma HLS stream depth=4 variable=dwordstrm_in // Set depth to non-default value

#pragma HLS dataflow

	const uint16_t bytes_in_len  = 4 * LENGTH;
	const uint16_t dwords_in_len =     LENGTH;
	const uint16_t bytes_out_len = 4 * LENGTH;

	strm_bytes2words<int32_t, sizeof(int32_t), false>(dwordstrm_in, strm_in, bytes_in_len);

	data_sorter<int32_t, LENGTH>(dwordstrm_in, dwordstrm_out);

	strm_words2bytes<int32_t, sizeof(int32_t), false>(strm_out,dwordstrm_out, bytes_out_len);

}
