/*************************************************************************
 *
 *              Author: b51
 *                Mail: b51live@gmail.com
 *            FileName: main.cc
 *
 *          Created On: Tue 02 Jul 2019 10:54:54 PM CST
 *     Licensed under The MIT License [see LICENSE for details]
 *
 ************************************************************************/

#include <glog/logging.h>
#include <math.h>
#include <algorithm>
#include <map>
#include <memory>
#include <opencv2/opencv.hpp>

#include "DarknetDetector.h"
#include "Detector.h"

typedef std::map<int, std::string> LabelNameMap;
typedef std::unordered_map<std::string, std::list<Object>> NamedObjsMap;
const LabelNameMap kLabelNameMap{
    {0, "robots"},
    {1, "posts"},
    {2, "balls"},
    {3, "target"},
};

static void MakePair(const std::vector<Object>& objs,
                     NamedObjsMap& named_objs_map) {
  for (const auto& ln : kLabelNameMap) {
    std::list<Object> objects;
    for (const auto& obj : objs) {
      if (obj.label == ln.first) {
        objects.push_back(obj);
      }
    }
    named_objs_map.insert(std::make_pair(ln.second, objects));
  }
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  FLAGS_minloglevel = google::INFO;
  FLAGS_logtostderr = true;
  FLAGS_colorlogtostderr = true;

  if (argc < 6)
    LOG(FATAL) << "Usage: " << argv[0] << " cfg model image ratio_fixed thresh";

  std::string prototxt(argv[1]);
  std::string model(argv[2]);
  cv::Mat img = cv::imread(argv[3]);
  int ratio_fixed = std::atoi(argv[4]);

  double object_thresh = std::atof(argv[5]);
  double nms_thresh = 0.4;
  double hier_thresh = 0.5;
  std::shared_ptr<Detector> detector = std::make_shared<DarknetDetector>();
  std::dynamic_pointer_cast<DarknetDetector>(detector)->SetNetParams(object_thresh, nms_thresh, hier_thresh);
  std::dynamic_pointer_cast<DarknetDetector>(detector)->LoadModel(prototxt, model);
  LOG(INFO) << "image: " << argv[3];

  int rz_w = 416;
  int rz_h = 416;  // 234?
  int w = img.cols;
  int h = img.rows;
  cv::Mat rzd_img(rz_h, rz_w, CV_8UC3, 128);  // padded image with 128
  if (!ratio_fixed) {
    cv::resize(img, rzd_img, cv::Size(rz_w, rz_h));
  } else {
    int new_w = w;
    int new_h = h;
    if (((float)rz_w / w) < ((float)rz_h / h)) {
      new_w = rz_w;
      new_h = (h * rz_w) / w;
    } else {
      new_h = rz_h;
      new_w = (w * rz_h) / h;
    }
    cv::Mat image_roi = rzd_img(
        cv::Rect(((rz_w - new_w) / 2), (rz_h - new_h) / 2, new_w, new_h));
    cv::resize(img, image_roi, cv::Size(new_w, new_h));
  }
  std::vector<Object> objs;
  NamedObjsMap named_objs_map;
  detector->Detect(rzd_img, w, h, objs);
  MakePair(objs, named_objs_map);

  // display
  for (const auto& no : named_objs_map) {
    std::string name = no.first;
    for (const auto obj : no.second) {
      cv::rectangle(img, cv::Rect(obj.x, obj.y, obj.w, obj.h),
                    cv::Scalar(255. / obj.label, 255, 0), 5);
      cv::putText(img, name, cv::Point(obj.x + obj.w / 2, obj.y + obj.h / 2),
                  cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar(255, 0, 0));
      cv::putText(img, std::to_string(obj.score), cv::Point(obj.x + obj.w / 2, obj.y),
                  cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar(255, 0, 0), 2);
    }
  }
  cv::imshow("disp", img);
  cv::waitKey(10000);
  return 1;
}
