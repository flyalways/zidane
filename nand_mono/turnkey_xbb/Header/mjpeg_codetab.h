code void (code * MjpegPhase[])(void) =
{
    mjpeg_idle,//0
    mjpeg_proc,//1
    mjpeg_playpause,//2
    mjpeg_stop,//3
	mjpeg_endfile,//4
	mjpeg_next,//5
	mjpeg_prev,//6
	mjpeg_fastfwd,//7
    mjpeg_fastrev,//8
	mjpeg_fffr_end,//9
	mjpeg_volup,//10
	mjpeg_voldn,//11
	mjpeg_back2uplevel//12
};