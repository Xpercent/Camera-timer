#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <thread>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <direct.h>
#include <vector>
#include<Windows.h>
#include <fstream>
#include <rapidjson/document.h>
using namespace cv;
using namespace std;
void img(int w, int h, int wcamera);
bool imggetTF();
void jsonpase();
vector<vector<int>> imgtimelist;
int c,w,h,wait_s,imgchoose;
int main() {
    jsonpase();
    while (true) {
        this_thread::sleep_for(chrono::seconds(wait_s));
        cout << 1;
        if (imgchoose == 0) {img(w, h,c);}
        else if (imgchoose == 1) {
            bool TF = imggetTF();
            if (TF == true) {
                img(w, h, c);
            }
        }
    }
    return 0;
}
void jsonpase() {
    ifstream file("config.json");// 读取文件内容到字符串
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    rapidjson::Document document;
    document.Parse(content.c_str());// 解析JSON数据
    c = document["c"].GetInt();
    w = document["w"].GetInt();
    h = document["h"].GetInt();
    wait_s= document["wait_s"].GetInt();
    imgchoose = document["imgchoose"].GetInt();
    rapidjson::Value& imgtimelistJson = document["imgtimelist"];
    for (rapidjson::SizeType i = 0; i < imgtimelistJson.Size(); i++) {
        const rapidjson::Value& innerArray = imgtimelistJson[i];
        int firstElement = innerArray[0].GetInt();
        int secondElement = innerArray[1].GetInt();
        imgtimelist.push_back({ firstElement, secondElement });
    }
}
bool imggetTF() {
    bool TF = false;
    SYSTEMTIME st;
    GetLocalTime(&st);
    int m = st.wMinute;
    int hm = st.wHour * 100 + m;
    int mdhm = (st.wMonth * 1000000) + (st.wDay * 10000) + hm;
    for (auto& task : imgtimelist) {
        int startTime = task[0];
        int endTime = task[1];
        int length = to_string(startTime).length();
        int nowtime;
        if (length == 1 || length == 2) {nowtime = m;}
        else if (length == 4 || length == 3) {nowtime = hm;}
        else{nowtime = mdhm;}
        if (startTime <= nowtime && nowtime <= endTime) {
            TF = true;
        }
    }
    return TF;
}
string creatfile() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    stringstream ss, ss2;
    ss <<st.wYear << "_"<< setw(2) << setfill('0') << st.wMonth << "_"
        << setw(2) << setfill('0') << st.wDay;

    ss2 << setw(2) << setfill('0') << st.wHour << "_"
        << setw(2) << setfill('0') << st.wMinute << "_"
        << setw(2) << setfill('0') << st.wSecond;
    string ymd = ss.str();
    string hms = ss2.str();

    string dataFolder = "data";
    string fullPath = dataFolder + "/" + ymd;
    if (_mkdir(dataFolder.c_str()) == -1) {}
    if (_mkdir(fullPath.c_str()) == -1) {}
    string imgname = fullPath + "/" + hms;
    return imgname;
}
void img(int w, int h, int wcamera) {
    VideoCapture cap(wcamera,CAP_DSHOW);// 打开摄像头
    cap.set(CAP_PROP_FRAME_WIDTH, w);// 设置摄像头分辨率为640x480
    cap.set(CAP_PROP_FRAME_HEIGHT, h);
    Mat frame;
    string savename = creatfile();
    cap >> frame;
    imwrite(savename + ".png", frame);
    cap.release();
}