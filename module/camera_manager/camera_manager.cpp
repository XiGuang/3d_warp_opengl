//
// Created by Admin on 2023/12/4.
//

#include "camera_manager.h"
#include <glog/logging.h>

void CameraManager::init(const std::string &data_path, const std::string &name_path, const glm::vec3& origin_position){
    _origin_position=origin_position;
    tinyxml2::XMLDocument camera_names;
    camera_names.LoadFile(name_path.c_str());
    auto root = camera_names.RootElement();
    for (auto *p = root->FirstChildElement(); p != nullptr; p = p->NextSiblingElement()) {
        _camera_names.emplace_back(p->GetText());
    }
    _camera_data.LoadFile(data_path.c_str());
}

CameraError CameraManager::updateCamera(NewCamera &camera) {
    if(++_current_camera>=_camera_names.size()){
        LOG(INFO)<< "No more camera";
        return CameraError::CAMERA_NO_MORE_CAMERA;
    }
    std::string name = _camera_names[_current_camera];
    auto root = _camera_data.RootElement();
    for(auto block=root->FirstChildElement("Block");block!= nullptr;block=block->NextSiblingElement("Block")){
        for(auto photo_groups=block->FirstChildElement("Photogroups");photo_groups!= nullptr;photo_groups=photo_groups->NextSiblingElement("Photogroups")){
            for(auto photo_group = photo_groups->FirstChildElement("Photogroup");photo_group!= nullptr;photo_group = photo_group->NextSiblingElement("Photogroup")){
                for(auto photo = photo_group->FirstChildElement("Photo");photo!= nullptr;photo = photo->NextSiblingElement("Photo")){
                    std::string path_name = photo->FirstChildElement("ImagePath")->GetText();
                    path_name=path_name.substr(path_name.find_last_of('/')+1);
                    if(path_name!=name){
                        continue;
                    }

                    int width,height;
                    width=photo_group->FirstChildElement("ImageDimensions")->FirstChildElement("Width")->IntText();
                    height=photo_group->FirstChildElement("ImageDimensions")->FirstChildElement("Height")->IntText();
                    double focal_length,sensor_size, principal_point_x, principal_point_y,k1,k2,k3,p1,p2;
                    focal_length = photo_group->FirstChildElement("FocalLength")->DoubleText();
                    sensor_size=photo_group->FirstChildElement("SensorSize")->DoubleText();
                    principal_point_x=photo_group->FirstChildElement("PrincipalPoint")->FirstChildElement("x")->DoubleText();
                    principal_point_y=photo_group->FirstChildElement("PrincipalPoint")->FirstChildElement("y")->DoubleText();
                    k1=photo_group->FirstChildElement("Distortion")->FirstChildElement("K1")->DoubleText();
                    k2=photo_group->FirstChildElement("Distortion")->FirstChildElement("K2")->DoubleText();
                    k3=photo_group->FirstChildElement("Distortion")->FirstChildElement("K3")->DoubleText();
                    p1=photo_group->FirstChildElement("Distortion")->FirstChildElement("P1")->DoubleText();
                    p2=photo_group->FirstChildElement("Distortion")->FirstChildElement("P2")->DoubleText();
                    auto pose = photo->FirstChildElement("Pose");
                    auto rotation = pose->FirstChildElement("Rotation");
                    auto position = pose->FirstChildElement("Center");
                    glm::mat3 rotation_matrix;
                    rotation_matrix[0][0]=rotation->FirstChildElement("M_00")->FloatText();
                    rotation_matrix[0][1]=rotation->FirstChildElement("M_01")->FloatText();
                    rotation_matrix[0][2]=rotation->FirstChildElement("M_02")->FloatText();
                    rotation_matrix[1][0]=rotation->FirstChildElement("M_10")->FloatText();
                    rotation_matrix[1][1]=rotation->FirstChildElement("M_11")->FloatText();
                    rotation_matrix[1][2]=rotation->FirstChildElement("M_12")->FloatText();
                    rotation_matrix[2][0]=rotation->FirstChildElement("M_20")->FloatText();
                    rotation_matrix[2][1]=rotation->FirstChildElement("M_21")->FloatText();
                    rotation_matrix[2][2]=rotation->FirstChildElement("M_22")->FloatText();
                    glm::vec3 position_vector;
                    position_vector[0]=position->FirstChildElement("x")->FloatText();
                    position_vector[1]=position->FirstChildElement("y")->FloatText();
                    position_vector[2]=position->FirstChildElement("z")->FloatText();
                    double sensor_size_y = sensor_size*height/width;
                    double fov = 2*atan(sensor_size_y/(2*focal_length)),aspect = (double)width/height;
                    camera=NewCamera(position_vector-_origin_position,_front,_up,rotation_matrix,fov,aspect,_near,_far);
                    return CameraError::CAMERA_NO_ERROR;
                }
            }
        }
    }

    LOG(INFO)<< "No camera named "<<name;
    return CameraError::CAMERA_NOT_FOUND_CORRESPONDING;
}


