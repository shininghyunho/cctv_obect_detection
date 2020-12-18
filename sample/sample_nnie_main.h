#ifndef __SAMPLE_SVP_MAIN_H__
#define __SAMPLE_SVP_MAIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include "hi_type.h"

#define LOCALTHRESH 0.01f
#define RECOGTHRESH 0.1f
#define DIGITMTHRESH 0.1f
#define YOLO3THRESH 0.1f
#define LOCALNMSTHRESH 0.5f
#define RECOGNMSTHRESH 0.5f
#define DIGITMNMSTHRESH 0.5f
#define YOLO3NMSTHRESH 0.5f

#define LOCALCLASSNUM 7
#define RECOGCLASSNUM 107
#define DIGITMCLASSNUM 13
#define YOLO3CLASSNUM 107


/******************************************************************************
* function : show Cnn sample
******************************************************************************/
void SAMPLE_SVP_NNIE_Cnn();

/******************************************************************************
* function : show Segnet sample
******************************************************************************/
void SAMPLE_SVP_NNIE_Segnet(void);

/******************************************************************************
* function : show FasterRcnn sample
******************************************************************************/
void SAMPLE_SVP_NNIE_FasterRcnn(void);

/******************************************************************************
* function : show fasterrcnn double roipooling sample
******************************************************************************/
void SAMPLE_SVP_NNIE_FasterRcnn_DoubleRoiPooling(void);

/******************************************************************************
* function : show RFCN sample
******************************************************************************/
void SAMPLE_SVP_NNIE_Rfcn(void);
/******************************************************************************
* function : Rfcn Read file
******************************************************************************/
void SAMPLE_SVP_NNIE_Rfcn_File(void);
/******************************************************************************
* function : show SSD sample
******************************************************************************/
void SAMPLE_SVP_NNIE_Ssd(void);
/******************************************************************************
* function : SSD For Localizer 박스 좌표 배열을 반환
******************************************************************************/
void SAMPLE_SVP_NNIE_Ssd_Localizer(void);
/******************************************************************************
* function : SSD For Recognizer 박스 좌표 배열을 반환
******************************************************************************/
void SAMPLE_SVP_NNIE_Ssd_Recognizer(void);
void SSD_ONE_IMG(char* imgName);
/******************************************************************************
* function : SSD N개 이미지 컨트롤러
******************************************************************************/
void SSD_CONTROLLER(void);
void Localizer_Load(void);
void Localizer_Inference(void);
void Recognizer_Load(void);
void Recognizer_Inference(void);
void DigitModel_Load(void);
void DigitModel_Inference(void);
void SAMPLE_SVP_NNIE_Ssd_Localizer_Test(void);

/******************************************************************************
* function : show YOLOV1 sample
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov1(void);

/******************************************************************************
* function : show YOLOV2 sample
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov2(void);

/******************************************************************************
* function : show YOLOV3 sample
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov3(void);

/******************************************************************************
* function : show Lstm sample
******************************************************************************/
void SAMPLE_SVP_NNIE_Lstm(void);

/******************************************************************************
* function : show Pvanet FasterRcnn sample
******************************************************************************/
void SAMPLE_SVP_NNIE_Pvanet(void);

/******************************************************************************
* function : Cnn sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Cnn_HandleSig(void);

/******************************************************************************
* function : Segnet sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Segnet_HandleSig(void);

/******************************************************************************
* function : fasterRcnn sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_FasterRcnn_HandleSig(void);

/******************************************************************************
* function : rfcn sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Rfcn_HandleSig(void);

/******************************************************************************
* function : rfcn sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Rfcn_HandleSig_File(void);
/******************************************************************************
* function : SSD sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Ssd_HandleSig(void);

/******************************************************************************
* function : Yolov1 sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov1_HandleSig(void);

/******************************************************************************
* function : Yolov2 sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov2_HandleSig(void);

/******************************************************************************
* function : Yolov3 sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov3_HandleSig(void);

/******************************************************************************
* function : Lstm sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Lstm_HandleSig(void);


/******************************************************************************
* function : Pvanet sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Pvanet_HandleSig(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SAMPLE_SVP_MAIN_H__ */
