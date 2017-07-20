//
//  main.cpp
//  TrackingSphero
//
//  Created by 田中恭友 on 2017/07/18.
//  Copyright © 2017年 example. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#pragma comment(lib, "opencv_core310.lib")
#pragma comment(lib, "opencv_imgcodecs310.lib")
#pragma comment(lib, "opencv_videoio310.lib")
#pragma comment(lib, "opencv_highgui310.lib")
#pragma comment(lib, "opencv_tracking310.lib")
#pragma comment(lib, "opencv_imgproc310.lib")

void myMouseCallback(int eventType, int x, int y, int flags, void* userdata)
{
    cv::Point2d *point = static_cast<cv::Point2d*> (userdata);
    
    if (eventType == cv::EVENT_LBUTTONDOWN) {
        std::cout << x << " , " << y << std::endl;
        point->x = x;
        point->y = y;
    }
}

void sendMessge(int sock, struct sockaddr_in addr, cv::Point2d nowPosition, cv::Point2d targetPosition){
    std::string message = "";
    message += "{ \"nowPosition\": {";
    message += "        \"x\" : " + std::to_string(nowPosition.x) + ",";
    message += "        \"y\" : " + std::to_string(nowPosition.y);
    message += "   },";
    message += "  \"targetPosition\": {";
    message += "        \"x\" : " + std::to_string(targetPosition.x) + ",";
    message += "        \"y\" : " + std::to_string(targetPosition.y);
    message += "   }";
    message += "}";
    
    sendto(sock, (char *)message.c_str(), message.length(), 0, (struct sockaddr *)&addr, sizeof(addr));
    //std::cout << message;
}

cv::Point2d getCenterOfROI(cv::Rect2d rect){
    return cv::Point2d(rect.x + rect.width / 2, rect.y + rect.height /2);
}

int main(int argc, char* argv[])
{
    // create socket
    int sock;
    struct sockaddr_in addr;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    // sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    
    bool save_video = false;
    int camera_id = 1;
    if (argc < 2) {
        //std::cout << "exe [video]" << std::endl;
        //return -1;
    } else {
        camera_id = atoi(argv[1]);
    }
    
    // Trackerの生成
    cv::Ptr<cv::Tracker> trackerKCF = cv::Tracker::create("KCF");
    
    cv::VideoCapture cap(camera_id);
    if (!cap.isOpened()) {
        std::cout << "ビデオが開けません。" << std::endl;
        return -1;
    }
    
    
    cv::Mat frame;
    while(1){
        cap >> frame;
        cv::imshow("select frame", frame);
        int key = cv::waitKey(30);
        if(key == 'q'){
            break;
        }
    }
    cv::destroyWindow("select frame");
    
    // 最初のフレームから追跡対象を選択
    cv::Rect2d roi = cv::selectROI("tracker", frame);
    
    cv::Mat target(frame, roi);
    std::cout << "(x, y, width, height) = (" << roi.x << "," << roi.y << "," << roi.width << "," << roi.height << ")" << std::endl;
    
    if (roi.width == 0 || roi.height == 0)
        return -1;
    
    // Trackerの初期化
    trackerKCF->init(frame, roi);
    
    // Trackerごとの色
    cv::Scalar colorkcf = cv::Scalar(0, 255, 0);
    
    // 動画保存の設定
    cv::VideoWriter writer;
    if(save_video){
        double fps = cap.get(CV_CAP_PROP_FPS);
        cv::Size size = cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        const int fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
        std::string filename = "output.avi";
        writer = cv::VideoWriter(filename, fourcc, fps, size);
    }
    
    // tracking!
    cv::Point2d nowPosition = getCenterOfROI(roi);
    cv::Point2d targetPosition = nowPosition;
    cv::setMouseCallback("tracker", myMouseCallback, &targetPosition);
    while (1) {
        cap >> frame;
        if (frame.empty()){
            break;
        }
        trackerKCF->update(frame, roi);
        nowPosition = getCenterOfROI(roi);

        //send data to server
        sendMessge(sock, addr, nowPosition, targetPosition);
        
        //visualize
        cv::rectangle(frame, roi, colorkcf, 1, 1);
        cv::circle(frame, targetPosition, 3, cv::Scalar(255, 0, 0), -1);
        cv::circle(frame, nowPosition, 3, cv::Scalar(0, 0, 255), -1);
        cv::putText(frame, "- KCF", cv::Point(5, 20), cv::FONT_HERSHEY_SIMPLEX, .5, colorkcf, 1, CV_AA);
        cv::imshow("tracker", frame);
        
        //output as video
        if(save_video){
            writer << frame;
        }
        
        int key = cv::waitKey(1);
        if (key == 0x1b){ //ESCキー
            break;
        }
    }
    
    return 0;
}
