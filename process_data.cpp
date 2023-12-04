//
// Created by Admin on 2023/12/3.
//

#include <filesystem>
#include <tinyxml2.h>
#include <glog/logging.h>
#include <gflags/gflags.h>

using namespace std;
using namespace tinyxml2;

DEFINE_bool(debug, false, "debug mode");

void selectXML(XMLDocument& doc,const vector<string>& names);

int main(int argc, char* argv[]){
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;  //是否打印到控制台
    FLAGS_alsologtostderr = true;  //打印到日志同时是否打印到控制台
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    vector<string> name_21,name_23;
    filesystem::path xml_path = R"(D:\Projects\3d_warp_opengl\xml)";

    // 判断文件是否存在
    if(!filesystem::exists(xml_path/"name"/"name_21.xml") || !filesystem::exists(xml_path/"name"/"name_23.xml")){
        LOG(INFO)<<"not found name xml, create it.";
        filesystem::path name_path=R"(D:\models\north_campus)";
        XMLDocument doc_name_21,doc_name_23;
        XMLDeclaration *declaration_21 = doc_name_21.NewDeclaration("xml version='1.0' encoding='utf-8' standalone='yes'");
        doc_name_21.InsertFirstChild(declaration_21);
        XMLDeclaration *declaration_23 = doc_name_23.NewDeclaration("xml version='1.0' encoding='utf-8' standalone='yes'");
        doc_name_23.InsertFirstChild(declaration_23);
        XMLElement *root = doc_name_21.NewElement("names");
        for(auto& p:filesystem::directory_iterator(name_path/"A")){
            string name = p.path().filename().string();
            name_21.emplace_back(name.substr(name.rfind('_')+1));
            XMLElement *xml_name = doc_name_21.NewElement("name");
            xml_name->SetText(name.substr(name.rfind('_')+1).c_str());
            root->InsertEndChild(xml_name);
        }
        doc_name_21.InsertEndChild(root);

        root = doc_name_23.NewElement("names");
        for(auto& p:filesystem::directory_iterator(name_path/"B")){
            string name = p.path().filename().string();
            name_23.emplace_back(name.substr(name.rfind('_')+1));
            XMLElement *xml_name = doc_name_23.NewElement("name");
            xml_name->SetText(name.substr(name.rfind('_')+1).c_str());
            root->InsertEndChild(xml_name);
        }
        doc_name_23.InsertEndChild(root);
        doc_name_21.SaveFile((xml_path/"name"/"name_21.xml").string().c_str());
        doc_name_23.SaveFile((xml_path/"name"/"name_23.xml").string().c_str());
    }else{
        LOG(INFO)<<"found name xml, init it.";
        XMLDocument doc_name_21,doc_name_23;
        if(doc_name_21.LoadFile((xml_path/"name"/"name_21.xml").string().c_str())!=XML_SUCCESS){
            LOG(ERROR) << "init xml failed "<<doc_name_21.ErrorName();
            return -1;
        }
        if(doc_name_23.LoadFile((xml_path/"name"/"name_23.xml").string().c_str())!=XML_SUCCESS){
            LOG(ERROR) << "init xml failed "<<doc_name_23.ErrorName();
            return -1;
        }
        XMLElement *root = doc_name_21.RootElement();
        for(XMLElement *p = root->FirstChildElement("name");p;p=p->NextSiblingElement("name")){
            name_21.emplace_back(p->GetText());
        }
        root = doc_name_23.RootElement();
        for(XMLElement *p = root->FirstChildElement("name");p;p=p->NextSiblingElement("name")){
            name_23.emplace_back(p->GetText());
        }
    }

    if(FLAGS_debug){
        return 0;
    }

    XMLDocument doc_21_1,doc_21_2,doc_23;
    if(doc_21_1.LoadFile((xml_path/"2021"/"0714-2000-114.xml").string().c_str())!=XML_SUCCESS){
        LOG(ERROR) << "init xml failed "<<doc_21_1.ErrorName();
        return -1;
    }
    if(doc_21_2.LoadFile((xml_path/"2021"/"2000-114.xml").string().c_str())!=XML_SUCCESS){
        LOG(ERROR) << "init xml failed "<<doc_21_2.ErrorName();
        return -1;
    }
    if(doc_23.LoadFile((xml_path/"2023"/"1.xml").string().c_str())!=XML_SUCCESS){
        LOG(ERROR) << "init xml failed "<<doc_23.ErrorName();
        return -1;
    }
    LOG(INFO) << "init xml success";

    selectXML(doc_21_1,name_21);
    selectXML(doc_21_2,name_21);
    selectXML(doc_23,name_23);

    // 将doc_21_1和doc_21_2合并
    XMLElement *root_21_1 = doc_21_1.RootElement();
    XMLElement *root_21_2 = doc_21_2.RootElement();
    for(XMLElement *p = root_21_2->FirstChildElement("Block");p;){
        XMLElement *tmp = p;
        p=p->NextSiblingElement("Block");
        root_21_1->InsertEndChild(tmp);
    }

    doc_21_1.SaveFile((xml_path/"result"/"result_21.xml").string().c_str());
    doc_23.SaveFile((xml_path/"result"/"result_23.xml").string().c_str());

    return 0;
}

void selectXML(XMLDocument &doc, const vector<string> &names) {
    XMLElement *root = doc.RootElement();
    for(XMLElement *p = root->FirstChildElement();p;){
        if(string(p->Name())!="Block"){
            XMLElement *tmp = p;
            p=p->NextSiblingElement();
            root->DeleteChild(tmp);
            continue;
        }
        for(XMLElement *q = p->FirstChildElement();q;){
            if(string(q->Name())!="Photogroups"){
                XMLElement *tmp = q;
                q=q->NextSiblingElement();
                p->DeleteChild(tmp);
                continue;
            }
            for(XMLElement *r = q->FirstChildElement("Photogroup");r;r=r->NextSiblingElement("Photogroup")){
                for(XMLElement *s = r->FirstChildElement("Photo");s;){
                    XMLElement *image_path = s->FirstChildElement("ImagePath");
                    if(image_path){
                        string path = image_path->GetText();
                        string name = path.substr(path.rfind('/')+1);
                        if(find(names.begin(),names.end(),name)==names.end()){
                            XMLElement *tmp = s;
                            s=s->NextSiblingElement("Photo");
                            r->DeleteChild(tmp);
                            continue;
                        }
                    }
                    s=s->NextSiblingElement("Photo");
                }
            }
            q=q->NextSiblingElement();
        }
        p=p->NextSiblingElement();
    }
}