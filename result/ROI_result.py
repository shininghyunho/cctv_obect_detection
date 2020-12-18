import numpy as np
import cv2
import os
import datetime

blue_color=(255,0,0)
Classes={
    "LOC":["background"
            ,"class0","class1","class2","class3","class4","class5"]
    ,"M13":["background","1","2","3","4","5","6","7","8","9","0","area","sign"]
    ,"M107":["background"
            , "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
            , "seoul", "daegu", "gwangju", "ulsan", "gyeonggi"
            , "chungbuk", "jeonbuk", "gyeongbuk", "jeju", "busan", "incheon"
            , "daejeon", "sejong", "gangwon", "chungnam", "jeonnam", "gyeongnam"
            , "ga", "na", "da", "ra", "ma", "ba", "sa"
            , "aa", "ja", "cha", "ka", "ta", "pa", "ha"
            , "goe", "noe", "doe", "roe", "moe", "boe", "soe", "aoe"
            , "joe", "choe", "koe", "toe", "poe", "hoe", "go"
            , "no", "do", "ro", "mo", "bo", "so", "oo", "jo", "cho"
            , "ko", "to", "po", "ho", "gu", "nu", "du", "ru", "mu"
            , "bu", "su", "wu", "ju", "chu", "ku", "tu"
            , "pu", "hu", "bae", "young", "-", "ke", "oikyo", "ge"
            , "ne", "de", "re", "me", "be", "se", "oe", "je"
            , "ch", "te", "pe", "he", "yuk", "hae", "gong", "guk", "hap"]
    ,"SSD":["background","plane","bicycle","bird","boat","bottle","bus","car","cat",
             "chair","cow","diningtable","dog","horse","motorbike","person",
             "pottedplant","sheep","sofa","train","tvmonitor"]
    ,"YOLO1":["background","plane","bicycle","bird","boat","bottle","bus","car","cat",
             "chair","cow","diningtable","dog","horse","motorbike","person",
             "pottedplant","sheep","sofa","train","tvmonitor"]
    ,"YOLO2":["background","Carclass","Vanclass","Truckclass",
               "Pedestrianclass","Cyclist"]
    ,"YOLO3":["background"
            , "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
            , "seoul", "daegu", "gwangju", "ulsan", "gyeonggi"
            , "chungbuk", "jeonbuk", "gyeongbuk", "jeju", "busan", "incheon"
            , "daejeon", "sejong", "gangwon", "chungnam", "jeonnam", "gyeongnam"
            , "ga", "na", "da", "ra", "ma", "ba", "sa"
            , "aa", "ja", "cha", "ka", "ta", "pa", "ha"
            , "goe", "noe", "doe", "roe", "moe", "boe", "soe", "aoe"
            , "joe", "choe", "koe", "toe", "poe", "hoe", "go"
            , "no", "do", "ro", "mo", "bo", "so", "oo", "jo", "cho"
            , "ko", "to", "po", "ho", "gu", "nu", "du", "ru", "mu"
            , "bu", "su", "wu", "ju", "chu", "ku", "tu"
            , "pu", "hu", "bae", "young", "-", "ke", "oikyo", "ge"
            , "ne", "de", "re", "me", "be", "se", "oe", "je"
            , "ch", "te", "pe", "he", "yuk", "hae", "gong", "guk", "hap"]
}

Sizes={
    "LOC":300
    ,"M13":300
    ,"M107":300
    ,"SSD":300
    ,"YOLO1":448
    ,"YOLO2":416
    ,"YOLO3":416
}
DETECTORS=["LOC","M13","M107","SSD","YOLO1","YOLO2","YOLO3"]

os.chdir('/home/hyunho/lab/cctv/result')
f=open("../nfs/nnie_copy/ROI.txt",'r')
lines=f.readlines()

detector=""
img_path=""
rois=[]
base_dir=os.getcwd()
#img=cv2.imread(os.path.join("/home/hyunho/lab/cctv/nfs/nnie_copy/",img_path))
for line in lines:
    # 디텍터, 이미지 이름, roi 뽑아내기
    line=line[:-1]
    print(line)
    if(line in DETECTORS):
        del rois[:]
        detector=line
    elif("img" in line):
        img_path=line.split(' ')[1]
    elif(line in '=============='):
        img=cv2.imread("/home/hyunho/lab/cctv/nfs/nnie_copy/"+img_path)
        try:
            img_resized=cv2.resize(img,dsize=(Sizes[detector],Sizes[detector]),interpolation=cv2.INTER_AREA)
        except Exception as e:
            print(str(e))
        for roi in rois:
            roi=roi.split()
            cv2.rectangle(img_resized,(int(roi[1]),int(roi[2])),(int(roi[3]),int(roi[4])),blue_color,2)
            class_num=int(roi[0])
            # 클래스 이름
            if class_num>=len(Classes[detector]):
                class_num=len(Classes[detector])-1
            cv2.putText(img_resized,Classes[detector][class_num],(int(roi[1]),int(roi[2])),cv2.FONT_HERSHEY_COMPLEX, 0.3, (0, 0, 0), 1)
            # 컨피던스
            cv2.putText(img_resized,str(float(roi[5])),(int(roi[3])-40,int(roi[2])),cv2.FONT_HERSHEY_COMPLEX, 0.3, (0, 0, 0), 1)
        os.chdir(os.path.join(base_dir,detector+"/test"))
        cv2.imwrite(datetime.datetime.now().strftime("%y%m%d_%H%M%S%f")+".jpg", img_resized)
    else:
        rois.append(line)
