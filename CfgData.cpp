//
// Created by yjw on 8/14/18.
//

#include <libxml/parser.h>
#include "CfgData.h"
#include "Logger.h"

CCfgData CCfgData::instance_;

CCfgData::CCfgData()
{
    // full path
    char c_path[1024];
    int len = readlink("/proc/self/exe", c_path, 1024);
    if (len < 0 || len >= 1024)
    {
        return;
    }

    c_path[len] = '\0';

    // program name
    char *p_name = strrchr(c_path, '/') + 1;
    program_name_ = p_name;

    // absolute path
    *p_name = '\0';
    full_path_ = c_path;

    // config path
    config_file_path_ = full_path_ + "configure.xml";
}

CCfgData::~CCfgData() = default;

void CCfgData::Fetch()
{
    xmlDocPtr xml_doc = nullptr;
    xmlNodePtr cur_node = nullptr;
    xmlChar *node_text = nullptr;

    //获取树形结构
    xml_doc = xmlParseFile(config_file_path_.c_str());
    if (xml_doc == nullptr)
    {
        SEVERE_LOG("Failed to parse xml file");
        return;
    }

    //获取根节点
    cur_node = xmlDocGetRootElement(xml_doc);
    if (cur_node == nullptr)
    {
        SEVERE_LOG("Root is empty");
        xmlFreeDoc(xml_doc);
        return;
    }

    if (0 != xmlStrcmp(cur_node->name, BAD_CAST "Config"))
    {
        SEVERE_LOG("The root is not Config");
        xmlFreeDoc(xml_doc);
        return;
    }

    //遍历处理根节点的每一个子节点
    cur_node = cur_node->xmlChildrenNode;
    while (cur_node != nullptr)
    {
        if (0 == xmlStrcmp(cur_node->name, BAD_CAST "LocalServerPort"))
        {
            node_text = xmlNodeGetContent(cur_node);
            local_server_port_ = atoi((char *) node_text);
        }

        if (node_text != nullptr)
        {
            xmlFree(node_text);
            node_text = nullptr;
        }

        cur_node = cur_node->next;
    }

    //
    xmlFreeDoc(xml_doc);
}