/* 
 * File:   FontManager.cc
 * Author: armstrong
 * 
 * Created on December 5, 2014, 10:14 AM
 */

#include "fonts.hh"

#ifdef IOSTREAM
#include <iostream>
using namespace std;
#endif

irr::map<int,irr::path>& FontFamily::getfonts(bool bold,bool italic){
  if(bold)
    if(italic)
      return bolditalicfonts;
    else
      return boldfonts;
  else
    if(italic)
      return italicfonts;
    else
      return regularfonts;
}

irr::path FontFamily::getFontName(int size,bool bold,bool italic){
  irr::map<int,irr::path>& fonts=getfonts(bold,italic);
  irr::map<int,irr::path>::Node* n=fonts.getRoot();
  irr::map<int,irr::path>::Node* nn=fonts.getRoot();
  while(nn!=0){//find from irrlicht map but store last node and only overwrite if we have a close node
    n=nn;
    const int& key=n->getKey();
    if (size == key)
      return n->getValue();
    else if (size < key)
      nn = n->getLeftChild();
    else //size > key
      nn = n->getRightChild();
  }
  if(n==0)//no entries in the map
    return irr::path();
  {
    irr::map<int,irr::path>::Iterator it(n);
    it--;
    nn=it.getNode();
  }
  if(nn==0 || n->getKey()+nn->getKey()<2*size)
    return n->getValue();
  else
    return nn->getValue();
}

bool FontFamily::addFont(irr::path fontname,int size,bool bold,bool italic){
  irr::map<int,irr::path>& fonts=getfonts(bold,italic);
  return fonts.insert(size,fontname);
}



FontFamily* VirtualFontSet::resolve(FontManager& m){
  return m.getFontFamily(realname);
}

inline bool parsebool(const wchar_t* str){
  return wcscmp(L"",str)&&wcscmp(L"0",str)&&wcscmp(L"false",str);
}

void FontManager::load(irr::stringc font){
  irr::path path("irrlicht/fonts/");
  path.append(font).append("/").append(font).append(".xml");
#ifdef IOSTREAM
  cout<<"loading font: "<<font.c_str()<<" from "<<path.c_str()<<endl;
#endif
  if(!fonts.insert(font,0))
    return;
  irr::IXMLReader* xml=fs->createXMLReader(path);
  if(xml==0)
    return;
  FontFamily* f=0;
  while(xml->read()){
    if(xml->getNodeType()==irr::EXN_ELEMENT){
      if (!wcscmp(L"member", xml->getNodeName())){
        if(f==0){
          f=new FontFamily();
          fonts.set(font,f);
        }
        bool bold=parsebool(xml->getAttributeValueSafe(L"bold"));
        bool italic=parsebool(xml->getAttributeValueSafe(L"italic"));
        int size=xml->getAttributeValueAsInt(L"size");
        irr::path fontpath("irrlicht/fonts/");
        fontpath.append(font).append("/").append(xml->getAttributeValueSafe(L"name")).append(".xml");
        f->addFont(fontpath,size,bold,italic);
      }else if(f==0 && !wcscmp(L"alias",xml->getNodeName())){
        fonts.set(font,new VirtualFontSet(xml->getAttributeValueSafe(L"target")));
        break;
      }
    }    
  }
  xml->drop();
}
FontManager::~FontManager(){
  for(irr::map<irr::stringc,FontSet*>::Iterator it=fonts.getIterator();
          !it.atEnd();it++){
    delete it->getValue();
    fs->drop();
    gui->drop();
  }
}