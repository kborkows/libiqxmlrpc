//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <stdexcept>
#include "except.h"
#include "xml_builder.h"

namespace iqxmlrpc {

namespace {

template <class T>
void
throwBuildError(T res, T err_res)
{
  if (res == err_res) {
    xmlErrorPtr err = xmlGetLastError();
    throw XmlBuild_error(err ? err->message : "unknown error");
  }
}

} // anonymous namespace

//
// XmlBuilder::Node
//

XmlBuilder::Node::Node(XmlBuilder& w, const char* name):
  ctx(w)
{
  const xmlChar* xname = reinterpret_cast<const xmlChar*>(name);
  throwBuildError(xmlTextWriterStartElement(ctx.writer, xname), -1);
}

XmlBuilder::Node::~Node()
{
  if (!std::uncaught_exception())
    xmlTextWriterEndElement(ctx.writer);
}

void
XmlBuilder::Node::set_textdata(const std::string& data)
{
  const xmlChar* xdata = reinterpret_cast<const xmlChar*>(data.c_str());
  throwBuildError(xmlTextWriterWriteString(ctx.writer, xdata), -1);
}

//
// XmlBuilder
//

XmlBuilder::XmlBuilder()
{
  buf = xmlBufferCreate();
  throwBuildError(writer = xmlNewTextWriterMemory(buf, 0), (xmlTextWriter*)0);
  throwBuildError(xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL), -1);
}

XmlBuilder::~XmlBuilder()
{
  xmlFreeTextWriter(writer);
  xmlBufferFree(buf);
}

void
XmlBuilder::stop()
{
  throwBuildError(xmlTextWriterEndDocument(writer), -1);
}

std::string
XmlBuilder::content() const
{
  const char* cdata = reinterpret_cast<const char*>(buf->content);
  return std::string(cdata, buf->use);
}

} // namespace iqxmlrpc
// vim:ts=2:sw=2:et
