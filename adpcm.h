// WAVE file header format
struct HEADER {
	unsigned char riff[4];				// (1-4)  RIFF string
	unsigned int overall_size;	    	// (5-8)  overall size of file in bytes
	unsigned char wave[4];				// (9-12) WAVE string
	unsigned char fmt_chunk_marker[4];	// (13-16) fmt string with trailing null char
	unsigned int length_of_fmt;			// (17-20) length of the format data
	unsigned short format_type;			// (21-22) format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	unsigned short channels;			// (23-24) no.of channels
	unsigned int sample_rate;			// (25-28) sampling rate (blocks per second)
	unsigned int byterate;				// (29-32) SampleRate * NumChannels * BitsPerSample/8
	unsigned short block_align;			// (33-34) NumChannels * BitsPerSample/8
	unsigned short bits_per_sample;		// (35-36) bits per sample, 8- 8bits, 16- 16 bits etc
} wavHeader;

// If format != PCM then there is extra data here
// format 17 is ima adpcm
struct EXTRA_DATA {
	unsigned short extraDataSize;		//  2 for IAM ADPCM
	unsigned short data_size;			//  Samples per block (IMA ADPCM)
	unsigned char subID[4];				//	"fact"
	unsigned int subSize;				//  size of next data
} extraData;

struct DATA_HEADER {
	unsigned char data_chunk_header [4];// DATA string or FLLR string
	unsigned int data_size;				// filesize - 60
} dataHeader;

static const int IndexTable[16] = {
   -1, -1, -1, -1,  // +0 - +3, decrease the step size
    2, 4, 6, 8,     // +4 - +7, increase the step size
   -1, -1, -1, -1,  // -0 - -3, decrease the step size
    2, 4, 6, 8,     // -4 - -7, increase the step size
};

static const int StepSizeTable[89] = {
   7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34,
   37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143,
   157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494,
   544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552,
   1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026,
   4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442,
   11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623,
   27086, 29794, 32767
};

const int diffTable[89][16] = {
{ 0,1,3,4,7,8,10,11,0,-1,-3,-4,-7,-8,-10,-11},
{ 1,3,5,7,9,11,13,15,-1,-3,-5,-7,-9,-11,-13,-15},
{ 1,3,5,7,10,12,14,16,-1,-3,-5,-7,-10,-12,-14,-16},
{ 1,3,6,8,11,13,16,18,-1,-3,-6,-8,-11,-13,-16,-18},
{ 1,3,6,8,12,14,17,19,-1,-3,-6,-8,-12,-14,-17,-19},
{ 1,4,7,10,13,16,19,22,-1,-4,-7,-10,-13,-16,-19,-22},
{ 1,4,7,10,14,17,20,23,-1,-4,-7,-10,-14,-17,-20,-23},
{ 1,4,8,11,15,18,22,25,-1,-4,-8,-11,-15,-18,-22,-25},
{ 2,6,10,14,18,22,26,30,-2,-6,-10,-14,-18,-22,-26,-30},
{ 2,6,10,14,19,23,27,31,-2,-6,-10,-14,-19,-23,-27,-31},
{ 2,6,11,15,21,25,30,34,-2,-6,-11,-15,-21,-25,-30,-34},
{ 2,7,12,17,23,28,33,38,-2,-7,-12,-17,-23,-28,-33,-38},
{ 2,7,13,18,25,30,36,41,-2,-7,-13,-18,-25,-30,-36,-41},
{ 3,9,15,21,28,34,40,46,-3,-9,-15,-21,-28,-34,-40,-46},
{ 3,10,17,24,31,38,45,52,-3,-10,-17,-24,-31,-38,-45,-52},
{ 3,10,18,25,34,41,49,56,-3,-10,-18,-25,-34,-41,-49,-56},
{ 4,12,21,29,38,46,55,63,-4,-12,-21,-29,-38,-46,-55,-63},
{ 4,13,22,31,41,50,59,68,-4,-13,-22,-31,-41,-50,-59,-68},
{ 5,15,25,35,46,56,66,76,-5,-15,-25,-35,-46,-56,-66,-76},
{ 5,16,27,38,50,61,72,83,-5,-16,-27,-38,-50,-61,-72,-83},
{ 6,18,31,43,56,68,81,93,-6,-18,-31,-43,-56,-68,-81,-93},
{ 6,19,33,46,61,74,88,101,-6,-19,-33,-46,-61,-74,-88,-101},
{ 7,22,37,52,67,82,97,112,-7,-22,-37,-52,-67,-82,-97,-112},
{ 8,24,41,57,74,90,107,123,-8,-24,-41,-57,-74,-90,-107,-123},
{ 9,27,45,63,82,100,118,136,-9,-27,-45,-63,-82,-100,-118,-136},
{ 10,30,50,70,90,110,130,150,-10,-30,-50,-70,-90,-110,-130,-150},
{ 11,33,55,77,99,121,143,165,-11,-33,-55,-77,-99,-121,-143,-165},
{ 12,36,60,84,109,133,157,181,-12,-36,-60,-84,-109,-133,-157,-181},
{ 13,39,66,92,120,146,173,199,-13,-39,-66,-92,-120,-146,-173,-199},
{ 14,43,73,102,132,161,191,220,-14,-43,-73,-102,-132,-161,-191,-220},
{ 16,48,81,113,146,178,211,243,-16,-48,-81,-113,-146,-178,-211,-243},
{ 17,52,88,123,160,195,231,266,-17,-52,-88,-123,-160,-195,-231,-266},
{ 19,58,97,136,176,215,254,293,-19,-58,-97,-136,-176,-215,-254,-293},
{ 21,64,107,150,194,237,280,323,-21,-64,-107,-150,-194,-237,-280,-323},
{ 23,70,118,165,213,260,308,355,-23,-70,-118,-165,-213,-260,-308,-355},
{ 26,78,130,182,235,287,339,391,-26,-78,-130,-182,-235,-287,-339,-391},
{ 28,85,143,200,258,315,373,430,-28,-85,-143,-200,-258,-315,-373,-430},
{ 31,94,157,220,284,347,410,473,-31,-94,-157,-220,-284,-347,-410,-473},
{ 34,103,173,242,313,382,452,521,-34,-103,-173,-242,-313,-382,-452,-521},
{ 38,114,191,267,345,421,498,574,-38,-114,-191,-267,-345,-421,-498,-574},
{ 42,126,210,294,379,463,547,631,-42,-126,-210,-294,-379,-463,-547,-631},
{ 46,138,231,323,417,509,602,694,-46,-138,-231,-323,-417,-509,-602,-694},
{ 51,153,255,357,459,561,663,765,-51,-153,-255,-357,-459,-561,-663,-765},
{ 56,168,280,392,505,617,729,841,-56,-168,-280,-392,-505,-617,-729,-841},
{ 61,184,308,431,555,678,802,925,-61,-184,-308,-431,-555,-678,-802,-925},
{ 68,204,340,476,612,748,884,1020,-68,-204,-340,-476,-612,-748,-884,-1020},
{ 74,223,373,522,672,821,971,1120,-74,-223,-373,-522,-672,-821,-971,-1120},
{ 82,246,411,575,740,904,1069,1233,-82,-246,-411,-575,-740,-904,-1069,-1233},
{ 90,271,452,633,814,995,1176,1357,-90,-271,-452,-633,-814,-995,-1176,-1357},
{ 99,298,497,696,895,1094,1293,1492,-99,-298,-497,-696,-895,-1094,-1293,-1492},
{ 109,328,547,766,985,1204,1423,1642,-109,-328,-547,-766,-985,-1204,-1423,-1642},
{ 120,360,601,841,1083,1323,1564,1804,-120,-360,-601,-841,-1083,-1323,-1564,-1804},
{ 132,397,662,927,1192,1457,1722,1987,-132,-397,-662,-927,-1192,-1457,-1722,-1987},
{ 145,436,728,1019,1311,1602,1894,2185,-145,-436,-728,-1019,-1311,-1602,-1894,-2185},
{ 160,480,801,1121,1442,1762,2083,2403,-160,-480,-801,-1121,-1442,-1762,-2083,-2403},
{ 176,528,881,1233,1587,1939,2292,2644,-176,-528,-881,-1233,-1587,-1939,-2292,-2644},
{ 194,582,970,1358,1746,2134,2522,2910,-194,-582,-970,-1358,-1746,-2134,-2522,-2910},
{ 213,639,1066,1492,1920,2346,2773,3199,-213,-639,-1066,-1492,-1920,-2346,-2773,-3199},
{ 234,703,1173,1642,2112,2581,3051,3520,-234,-703,-1173,-1642,-2112,-2581,-3051,-3520},
{ 258,774,1291,1807,2324,2840,3357,3873,-258,-774,-1291,-1807,-2324,-2840,-3357,-3873},
{ 284,852,1420,1988,2556,3124,3692,4260,-284,-852,-1420,-1988,-2556,-3124,-3692,-4260},
{ 312,936,1561,2185,2811,3435,4060,4684,-312,-936,-1561,-2185,-2811,-3435,-4060,-4684},
{ 343,1030,1717,2404,3092,3779,4466,5153,-343,-1030,-1717,-2404,-3092,-3779,-4466,-5153},
{ 378,1134,1890,2646,3402,4158,4914,5670,-378,-1134,-1890,-2646,-3402,-4158,-4914,-5670},
{ 415,1246,2078,2909,3742,4573,5405,6236,-415,-1246,-2078,-2909,-3742,-4573,-5405,-6236},
{ 457,1372,2287,3202,4117,5032,5947,6862,-457,-1372,-2287,-3202,-4117,-5032,-5947,-6862},
{ 503,1509,2516,3522,4529,5535,6542,7548,-503,-1509,-2516,-3522,-4529,-5535,-6542,-7548},
{ 553,1660,2767,3874,4981,6088,7195,8302,-553,-1660,-2767,-3874,-4981,-6088,-7195,-8302},
{ 608,1825,3043,4260,5479,6696,7914,9131,-608,-1825,-3043,-4260,-5479,-6696,-7914,-9131},
{ 669,2008,3348,4687,6027,7366,8706,10045,-669,-2008,-3348,-4687,-6027,-7366,-8706,-10045},
{ 736,2209,3683,5156,6630,8103,9577,11050,-736,-2209,-3683,-5156,-6630,-8103,-9577,-11050},
{ 810,2431,4052,5673,7294,8915,10536,12157,-810,-2431,-4052,-5673,-7294,-8915,-10536,-12157},
{ 891,2674,4457,6240,8023,9806,11589,13372,-891,-2674,-4457,-6240,-8023,-9806,-11589,-13372},
{ 980,2941,4902,6863,8825,10786,12747,14708,-980,-2941,-4902,-6863,-8825,-10786,-12747,-14708},
{ 1078,3235,5393,7550,9708,11865,14023,16180,-1078,-3235,-5393,-7550,-9708,-11865,-14023,-16180},
{ 1186,3559,5932,8305,10679,13052,15425,17798,-1186,-3559,-5932,-8305,-10679,-13052,-15425,-17798},
{ 1305,3915,6526,9136,11747,14357,16968,19578,-1305,-3915,-6526,-9136,-11747,-14357,-16968,-19578},
{ 1435,4306,7178,10049,12922,15793,18665,21536,-1435,-4306,-7178,-10049,-12922,-15793,-18665,-21536},
{ 1579,4737,7896,11054,14214,17372,20531,23689,-1579,-4737,-7896,-11054,-14214,-17372,-20531,-23689},
{ 1737,5211,8686,12160,15636,19110,22585,26059,-1737,-5211,-8686,-12160,-15636,-19110,-22585,-26059},
{ 1911,5733,9555,13377,17200,21022,24844,28666,-1911,-5733,-9555,-13377,-17200,-21022,-24844,-28666},
{ 2102,6306,10511,14715,18920,23124,27329,31533,-2102,-6306,-10511,-14715,-18920,-23124,-27329,-31533},
{ 2312,6937,11562,16187,20812,25437,30062,34687,-2312,-6937,-11562,-16187,-20812,-25437,-30062,-34687},
{ 2543,7630,12718,17805,22893,27980,33068,38155,-2543,-7630,-12718,-17805,-22893,-27980,-33068,-38155},
{ 2798,8394,13990,19586,25183,30779,36375,41971,-2798,-8394,-13990,-19586,-25183,-30779,-36375,-41971},
{ 3077,9232,15388,21543,27700,33855,40011,46166,-3077,-9232,-15388,-21543,-27700,-33855,-40011,-46166},
{ 3385,10156,16928,23699,30471,37242,44014,50785,-3385,-10156,-16928,-23699,-30471,-37242,-44014,-50785},
{ 3724,11172,18621,26069,33518,40966,48415,55863,-3724,-11172,-18621,-26069,-33518,-40966,-48415,-55863},
{ 4095,12286,20478,28669,36862,45053,53245,61436,-4095,-12286,-20478,-28669,-36862,-45053,-53245,-61436},
};

int prevsample = 0;
int previndex = 0;


inline signed short ImaAdpcmDecode(unsigned char code) {

	int predsample = (signed short) prevsample;
	int index = previndex;

    predsample += diffTable[index][code];

	if (predsample < -0x8000){
	    predsample = -0x8000;
	}
	if (predsample > 0x7fff){
	    predsample = 0x7fff;
	}

	index = index + IndexTable[code];

	if (index < 0){
	    index = 0;
	}
	if (index > 88){
	    index = 88;
	}

	prevsample = predsample;
	previndex = index;

	return predsample;
}

void readADPCMHeader(){
        // lets open the file here
    if(wavFile.openRO(adpcmFilename)){
        wavFile.read(&wavHeader.riff, 4);
        wavFile.read(&wavHeader.overall_size, 4);
        wavFile.read(&wavHeader.wave, 4);
        wavFile.read(&wavHeader.fmt_chunk_marker, 4);
        wavFile.read(&wavHeader.length_of_fmt, 4);
        wavFile.read(&wavHeader.format_type, 2);
        wavFile.read(&wavHeader.channels, 2);
        wavFile.read(&wavHeader.sample_rate, 4);
        wavFile.read(&wavHeader.byterate, 4);
        wavFile.read(&wavHeader.block_align, 2);
        wavFile.read(&wavHeader.bits_per_sample, 2);

        // if file is any other than PCM, theren there IS ALWAYS an extended portion here
        // even if the size is zero

        // 17 - IMA ADPCM, 2 = Microsoft ADPCM
        if(wavHeader.format_type == 17){

            wavFile.read(&extraData.extraDataSize, 2);
            wavFile.read(&extraData.data_size, 2);
            wavFile.read(&extraData.subID, 4);
            wavFile.read(&extraData.subSize, 4);
            // should read then next 'extraData.subSize' number of bytes, but I wont use them, so just skip.
            char tempText[4];
            wavFile.read(&tempText[0], 4);
        }

        wavFile.read(&dataHeader.data_chunk_header, 4);
        wavFile.read(&dataHeader.data_size, 4);
    }

}


