// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ResponseHeader.proto

#ifndef PROTOBUF_ResponseHeader_2eproto__INCLUDED
#define PROTOBUF_ResponseHeader_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace com {
namespace tonyemu {
namespace proto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_ResponseHeader_2eproto();
void protobuf_AssignDesc_ResponseHeader_2eproto();
void protobuf_ShutdownFile_ResponseHeader_2eproto();

class ResponseHeader;

// ===================================================================

class ResponseHeader : public ::google::protobuf::Message {
 public:
  ResponseHeader();
  virtual ~ResponseHeader();

  ResponseHeader(const ResponseHeader& from);

  inline ResponseHeader& operator=(const ResponseHeader& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ResponseHeader& default_instance();

  void Swap(ResponseHeader* other);

  // implements Message ----------------------------------------------

  ResponseHeader* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ResponseHeader& from);
  void MergeFrom(const ResponseHeader& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required uint32 state_code = 1;
  inline bool has_state_code() const;
  inline void clear_state_code();
  static const int kStateCodeFieldNumber = 1;
  inline ::google::protobuf::uint32 state_code() const;
  inline void set_state_code(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:com.tonyemu.proto.ResponseHeader)
 private:
  inline void set_has_state_code();
  inline void clear_has_state_code();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 state_code_;
  friend void  protobuf_AddDesc_ResponseHeader_2eproto();
  friend void protobuf_AssignDesc_ResponseHeader_2eproto();
  friend void protobuf_ShutdownFile_ResponseHeader_2eproto();

  void InitAsDefaultInstance();
  static ResponseHeader* default_instance_;
};
// ===================================================================


// ===================================================================

// ResponseHeader

// required uint32 state_code = 1;
inline bool ResponseHeader::has_state_code() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ResponseHeader::set_has_state_code() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ResponseHeader::clear_has_state_code() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ResponseHeader::clear_state_code() {
  state_code_ = 0u;
  clear_has_state_code();
}
inline ::google::protobuf::uint32 ResponseHeader::state_code() const {
  // @@protoc_insertion_point(field_get:com.tonyemu.proto.ResponseHeader.state_code)
  return state_code_;
}
inline void ResponseHeader::set_state_code(::google::protobuf::uint32 value) {
  set_has_state_code();
  state_code_ = value;
  // @@protoc_insertion_point(field_set:com.tonyemu.proto.ResponseHeader.state_code)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace proto
}  // namespace tonyemu
}  // namespace com

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_ResponseHeader_2eproto__INCLUDED
