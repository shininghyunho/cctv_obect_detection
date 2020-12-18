#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_nnie_main.h"
#include "inference/proc.h"

static char **s_ppChCmdArgv = NULL;
/******************************************************************************
* function : to process abnormal case
******************************************************************************/
#ifndef __HuaweiLite__
void SAMPLE_SVP_HandleSig(int s32Signo){
    signal(SIGINT,SIG_IGN);
    signal(SIGTERM,SIG_IGN);

    if (SIGINT == s32Signo || SIGTERM == s32Signo){
        switch (*s_ppChCmdArgv[1]){
            case '0':
                {
                   SAMPLE_SVP_NNIE_Rfcn_HandleSig();
                }
                break;
            case '1':
                {
                   SAMPLE_SVP_NNIE_Segnet_HandleSig();
                }
                break;
            case '2':
                {
                   SAMPLE_SVP_NNIE_FasterRcnn_HandleSig();
                }
                break;
            case '3':
                {
                   SAMPLE_SVP_NNIE_FasterRcnn_HandleSig();
                }
                break;
            case '4':
                {
                   SAMPLE_SVP_NNIE_Cnn_HandleSig();
                }
                break;
            case '5':
                {
                   SAMPLE_SVP_NNIE_Ssd_HandleSig();
                }
                break;
            case '6':
                {
                   SAMPLE_SVP_NNIE_Yolov1_HandleSig();
                }
                break;
            case '7':
                {
                   SAMPLE_SVP_NNIE_Yolov2_HandleSig();
                }
                break;
            case '8':
                {
                   SAMPLE_SVP_NNIE_Yolov3_HandleSig();
                }
                break;
            case '9':
                {
                   SAMPLE_SVP_NNIE_Lstm_HandleSig();
                }
		       break;
    	    case 'a':
    		   {
    		  	SAMPLE_SVP_NNIE_Pvanet_HandleSig();
    		   }
               break;
            case 'b':
               {
                SAMPLE_SVP_NNIE_Rfcn_HandleSig_File();
               }
              break;
           default :
            {
            }
            break;
        }
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}
#endif

/******************************************************************************
* function : nnie sample
******************************************************************************/
#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    int s32Ret = HI_SUCCESS;

    printf("Test Start\n");
    if(argc>1){
        // 한장 테스트
        SSD_ONE_IMG(argv[1]);
    }
    else{
        // 여러장 테스트
        SSD_CONTROLLER();
    }
    printf("Test End\n");
    s_ppChCmdArgv = argv;
#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_SVP_HandleSig);
    signal(SIGTERM, SAMPLE_SVP_HandleSig);
#endif
    return s32Ret;
}