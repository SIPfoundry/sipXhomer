// Copyright (c) 2012 eZuce, Inc. All rights reserved.
// Contributed to SIPfoundry under a Contributor Agreement
//
// This software is free software; you can redistribute it and/or modify it under
// the terms of the Affero General Public License (AGPL) as published by the
// Free Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This software is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.

#ifndef HEPMESSAGE_H
#define	HEPMESSAGE_H


#include <string>
#include <boost/asio.hpp>
#include <sstream>

class HEPMessage
{
public:

  enum VendorId
  {
    Generic = 0x0000, // No specific vendor, generic chunk types, see above
    FreeSwitch = 0x0001, // FreeSWITCH (www.freeswitch.org)
    Kamailio = 0x0002, // Kamailio/SER (www.kamailio.org)
    OpenSips = 0x0003, // OpenSIPS (www.opensips.org)
    Asterisk = 0x0004, // Asterisk (www.asterisk.org)
    Homer = 0x0005, // Homer Project (http://www.sipcapture.org)
    SipX = 0x0006 // SipXecs (www.sipfoundry.org/)
  };

  enum ChunkType
  {
    Unknown = 0x0000,
    IpProtoFamily,
    IpProtoId,
    Ip4SrcAddress,
    Ip4DestAddress,
    Ip6SrcAddress,
    Ip6DestAddress,
    SrcPort,
    DestPort,
    TimeStamp,
    TimeStampMicroOffset,
    ProtocolType,
    Data,
    MaxType
  };
  
  enum ProtocolId
  {
      TCP = 6,
      UDP = 17,
  };
  
  enum ProtocolFamily
  {
      IpV4 = 2,
      IpV6 = 6
  };
  
  enum ProtocolType
  {
      SIP = 0x01,
      H323,
      RTP,
      MGCP,
      SIGTRAN
  };



  HEPMessage();
  explicit HEPMessage(const std::string& message);
  explicit HEPMessage(const HEPMessage& message);

  HEPMessage& operator=(const std::string& message);
  HEPMessage& operator=(const HEPMessage& message);
  void swap(HEPMessage& message);

  const ::uint8_t& getIpProtoFamily() const;
  void setIpProtoFamily(const ::uint8_t& ipProtoFamily);

  const ::uint8_t& getIpProtoId() const;
  void setIpProtoId(const ::uint8_t& ipProtoId);

  const boost::asio::ip::address_v4& getIp4SrcAddress() const;
  void setIp4SrcAddress(const boost::asio::ip::address_v4& ip4SrcAddress);
  void setIp4SrcAddress(const boost::asio::ip::address_v4::bytes_type& ip4SrcAddress);
  void setIp4SrcAddress(const std::string& ip4SrcAddress);

  const boost::asio::ip::address_v4& getIp4DestAddress() const;
  void setIp4DestAddress(const boost::asio::ip::address_v4& ip4DestAddress);
  void setIp4DestAddress(const boost::asio::ip::address_v4::bytes_type& ip4DestAddress);
  void setIp4DestAddress(const std::string& ip4DestAddress);

  const boost::asio::ip::address_v6& getIp6SrcAddress() const;
  void setIp6SrcAddress(const boost::asio::ip::address_v6& ip6SrcAddress);
  void setIp6SrcAddress(const boost::asio::ip::address_v6::bytes_type& ip6SrcAddress);
  void setIp6SrcAddress(const std::string& ip6SrcAddress);

  const boost::asio::ip::address_v6& getIp6DestAddress() const;
  void setIp6DestAddress(const boost::asio::ip::address_v6& ip6DestAddress);
  void setIp6DestAddress(const boost::asio::ip::address_v6::bytes_type& ip6DestAddress);
  void setIp6DestAddress(const std::string& ip6DestAddress);

  const ::uint16_t& getSrcPort() const;
  void setSrcPort(const ::uint16_t& srcPort);

  const ::uint16_t& getDestPort() const;
  void setDestPort(const ::uint16_t& destPort);

  const ::uint32_t& getTimeStamp() const;
  void setTimeStamp(const ::uint32_t& timeStamp);

  const ::uint32_t& getTimeStampMicroOffset() const;
  void setTimeStampMicroOffset(const ::uint32_t& timeStampMicroOffset);

  const ::uint8_t& getProtocolType() const;
  void setProtocolType(const ::uint8_t& protocolType);

  const std::string& getData() const;
  void setData(const std::string& data);

  bool setChunk(unsigned short vendorId, unsigned short typeId, const std::string& data);

  bool encode(std::ostream& strm);

  bool parse(const std::string& data);
 
private:
  ::uint8_t _ipProtoFamily;
  ::uint8_t _ipProtoId;
  boost::asio::ip::address_v4 _ip4SrcAddress;
  boost::asio::ip::address_v4 _ip4DestAddress;
  boost::asio::ip::address_v6 _ip6SrcAddress;
  boost::asio::ip::address_v6 _ip6DestAddress;
  ::uint16_t _srcPort;
  ::uint16_t _destPort;
  ::uint32_t _timeStamp;
  ::uint32_t _timeStampMicroOffset;
  ::uint8_t _protocolType;
  std::string _data;
};

//
// Inlines
//

inline HEPMessage::HEPMessage() :
  _ipProtoFamily(0),
  _ipProtoId(0),
  _ip4SrcAddress(),
  _ip4DestAddress(),
  _ip6SrcAddress(),
  _ip6DestAddress(),
  _srcPort(0),
  _destPort(0),
  _timeStamp(0),
  _timeStampMicroOffset(0),
  _protocolType(0),
  _data()
{

}

inline HEPMessage::HEPMessage(const std::string& message)
{
  parse(message);
}

inline HEPMessage::HEPMessage(const HEPMessage& message)
{
  _ipProtoFamily = message._ipProtoFamily;
  _ipProtoId = message._ipProtoId;
  _ip4SrcAddress = message._ip4SrcAddress;
  _ip4DestAddress = message._ip4DestAddress;
  _ip6SrcAddress = message._ip6SrcAddress;
  _ip6DestAddress = message._ip6DestAddress;
  _srcPort = message._srcPort;
  _destPort = message._destPort;
  _timeStamp = message._timeStamp;
  _timeStampMicroOffset = message._timeStampMicroOffset;
  _protocolType = message._protocolType;
  _data = message._data;
}

inline HEPMessage& HEPMessage::operator=(const std::string& message)
{
  parse(message);
  return *this;
}

inline HEPMessage& HEPMessage::operator=(const HEPMessage& message)
{
  HEPMessage clonable(message);
  swap(clonable);
  return *this;
}

inline void HEPMessage::swap(HEPMessage& message)
{
  std::swap(_ipProtoFamily, message._ipProtoFamily);
  std::swap(_ipProtoId, message._ipProtoId);
  std::swap(_ip4SrcAddress, message._ip4SrcAddress);
  std::swap(_ip4DestAddress, message._ip4DestAddress);
  std::swap(_ip6SrcAddress, message._ip6SrcAddress);
  std::swap(_ip6DestAddress, message._ip6DestAddress);
  std::swap(_srcPort, message._srcPort);
  std::swap(_destPort, message._destPort);
  std::swap(_timeStamp, message._timeStamp);
  std::swap(_timeStampMicroOffset, message._timeStampMicroOffset);
  std::swap(_protocolType, message._protocolType);
  std::swap(_data, message._data);
}

inline bool HEPMessage::parse(const std::string& message)
{
  enum ParserState
  {
    VendorIdParse,
    TypeIdParse,
    ChunkLengthParse,
    ChunkParse,
    MessageEnd
  }parserState = VendorIdParse;

  ::uint8_t vendorId[2];
  ::uint8_t typeId[2];
  ::uint8_t chunkLength[2];
  std::string data;

  for (std::string::const_iterator iter = message.begin(); iter != message.end();)
  {

    switch(parserState)
    {
      case VendorIdParse:
        if (iter == message.end())
          break;
        vendorId[0] = static_cast<uint8_t>(*iter);
        iter++;
        if (iter != message.end())
        {
          vendorId[1] = static_cast<uint8_t>(*iter);;
          parserState = TypeIdParse;
          iter++;
        }
        break;
      case TypeIdParse:
        if (iter == message.end())
          break;
        typeId[0] = static_cast<uint8_t>(*iter);
        iter++;
        if (iter != message.end())
        {
          typeId[1] = static_cast<uint8_t>(*iter);;
          parserState = ChunkLengthParse;
          iter++;
        }
        break;
      case ChunkLengthParse:
        if (iter == message.end())
          break;
          chunkLength[0] = static_cast<uint8_t>(*iter);
        iter++;
        if (iter != message.end())
        {
          chunkLength[1] = static_cast<uint8_t>(*iter);
          parserState = ChunkParse;
          iter++;
          data.clear();
          data.reserve(*(reinterpret_cast<std::size_t*>(chunkLength)));
        }
        break;
      case ChunkParse:
        if (iter == message.end())
          break;
        
        data.push_back(*iter);
        
        if (*(reinterpret_cast<std::size_t*>(chunkLength)) == data.size())
        {
          parserState = VendorIdParse;
          iter++;
          setChunk(
                  *(reinterpret_cast<unsigned short*>(vendorId)),
                  *(reinterpret_cast<unsigned short*>(typeId)),
                  data);
        }
        else
        {
          iter++;
        }
        break;
      case MessageEnd:
          break;
    }
  }
  return true;
}

inline bool HEPMessage::setChunk(unsigned short vendorId, unsigned short typeId, const std::string& data)
{
  if (typeId <= Unknown || typeId >= MaxType)
    return false;
  //
  // for now we will only be concerned about generic types
  // 
  if (vendorId == Generic)
  {
    boost::asio::ip::address_v4::bytes_type ip4bytes;
    boost::asio::ip::address_v6::bytes_type ip6bytes;
    switch (typeId)
    {
      
      case IpProtoFamily:
        if (data.size() == sizeof(::uint8_t))
          _ipProtoFamily = *(reinterpret_cast<uint8_t*>(const_cast<char*>(data.data())));
        break;
      case IpProtoId:
        if (data.size() == sizeof(::uint8_t))
          _ipProtoId = *(reinterpret_cast<uint8_t*>(const_cast<char*>(data.data())));
        break;
      case Ip4SrcAddress:
        if (data.size() == ip4bytes.size())
        {
          for (size_t i = 0; i < ip4bytes.size(); i++)
            ip4bytes[i] = data[i];
          _ip4SrcAddress = boost::asio::ip::address_v4(ip4bytes);
        }
        break;
      case Ip4DestAddress:
        if (data.size() == ip4bytes.size())
        {
          for (size_t i = 0; i < ip4bytes.size(); i++)
            ip4bytes[i] = data[i];
          _ip4DestAddress = boost::asio::ip::address_v4(ip4bytes);
        }
        break;
      case Ip6SrcAddress:
        if (data.size() == ip6bytes.size())
        {
          for (size_t i = 0; i < ip6bytes.size(); i++)
            ip6bytes[i] = data[i];
          _ip6SrcAddress = boost::asio::ip::address_v6(ip6bytes);
        }
        break;
      case Ip6DestAddress:
        if (data.size() == ip6bytes.size())
        {
          for (size_t i = 0; i < ip6bytes.size(); i++)
            ip6bytes[i] = data[i];
          _ip6DestAddress = boost::asio::ip::address_v6(ip6bytes);
        }
        break;
      case SrcPort:
        if (data.size() == sizeof(::uint16_t))
          _srcPort = *reinterpret_cast<uint16_t*>(const_cast<char*>(data.data()));
        break;
      case DestPort:
        if (data.size() == sizeof(::uint16_t))
          _destPort = *reinterpret_cast<uint16_t*>(const_cast<char*>(data.data()));
        break;
      case TimeStamp:
        if (data.size() == sizeof(::uint32_t))
          _timeStamp = *reinterpret_cast<uint32_t*>(const_cast<char*>(data.data()));
        break;
      case TimeStampMicroOffset:
        if (data.size() == sizeof(::uint32_t))
          _timeStampMicroOffset = *reinterpret_cast<uint32_t*>(const_cast<char*>(data.data()));
        break;
      case ProtocolType:
        if (data.size() == sizeof(::uint8_t))
          _protocolType = *reinterpret_cast<uint8_t*>(const_cast<char*>(data.data()));
        break;
      case Data:
        _data = data;
        break;
    }
  }

  return true;
}

inline bool HEPMessage::encode(std::ostream& strm)
{
  ::uint16_t vendorId = 0x0000;
  ::uint16_t dataSize = 0;
  
  ::uint16_t ipProtoFamilyId = IpProtoFamily;
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&ipProtoFamilyId, sizeof(::uint16_t));
  dataSize = sizeof(::uint8_t);
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)&_ipProtoFamily, sizeof(::uint8_t));

  ::uint16_t ipProtoIdId = IpProtoId;
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&ipProtoIdId, sizeof(::uint16_t));
  dataSize = sizeof(::uint8_t);
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)&_ipProtoId, sizeof(::uint8_t));

  ::uint16_t ip4SrcAddressId = Ip4SrcAddress;
  boost::asio::ip::address_v4::bytes_type ip4SrcAddressIdBytes = _ip4SrcAddress.to_bytes();
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&ip4SrcAddressId, sizeof(::uint16_t));
  dataSize = 4;
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)ip4SrcAddressIdBytes.data(), ip4SrcAddressIdBytes.size());

  ::uint16_t ip4DestAddressId = Ip4DestAddress;
  boost::asio::ip::address_v4::bytes_type ip4DestAddressBytes = _ip4DestAddress.to_bytes();
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&ip4DestAddressId, sizeof(::uint16_t));
  dataSize = 4;
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)ip4DestAddressBytes.data(), ip4DestAddressBytes.size());

  ::uint16_t ip6SrcAddressId = Ip6SrcAddress;
  boost::asio::ip::address_v6::bytes_type ip6SrcAddressIdBytes = _ip6SrcAddress.to_bytes();
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&ip6SrcAddressId, sizeof(::uint16_t));
  dataSize = 16;
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)ip6SrcAddressIdBytes.data(), ip6SrcAddressIdBytes.size());

  ::uint16_t ip6DestAddressId = Ip6DestAddress;
  boost::asio::ip::address_v6::bytes_type ip6DestAddressBytes = _ip6DestAddress.to_bytes();
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&ip6DestAddressId, sizeof(::uint16_t));
  dataSize = 16;
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)ip6DestAddressBytes.data(), ip6DestAddressBytes.size());

  ::uint16_t srcPortId = SrcPort;
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&srcPortId, sizeof(::uint16_t));
  dataSize = sizeof(::uint16_t);
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)&_srcPort, sizeof(::uint16_t));

  ::uint16_t destPortId = DestPort;
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&destPortId, sizeof(::uint16_t));
  dataSize = sizeof(::uint16_t);
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)&_destPort, sizeof(::uint16_t));

  ::uint16_t timeStampId = TimeStamp;
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&timeStampId, sizeof(::uint16_t));
  dataSize = sizeof(::uint32_t);
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)&_timeStamp, sizeof(::uint32_t));

  ::uint16_t timeStampMicroOffsetId = TimeStampMicroOffset;
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&timeStampMicroOffsetId, sizeof(::uint16_t));
  dataSize = sizeof(::uint32_t);
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)&_timeStampMicroOffset, sizeof(::uint32_t));

  ::uint16_t protocolTypeId = ProtocolType;
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&protocolTypeId, sizeof(::uint16_t));
  dataSize = sizeof(::uint8_t);
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write((const char*)&_protocolType, sizeof(::uint8_t));

  ::uint16_t dataId = Data;
  strm.write((const char*)&vendorId, sizeof(::uint16_t));
  strm.write((const char*)&dataId, sizeof(::uint16_t));
  dataSize = _data.size();
  strm.write((const char*)&dataSize, sizeof(::uint16_t));
  strm.write(_data.c_str(), _data.size());

  return true;
}

inline const ::uint8_t& HEPMessage::getIpProtoFamily() const
{
  return _ipProtoFamily;
}

inline void HEPMessage::setIpProtoFamily(const ::uint8_t& ipProtoFamily)
{
  _ipProtoFamily = ipProtoFamily;
}

inline const ::uint8_t& HEPMessage::getIpProtoId() const
{
  return _ipProtoId;
}

inline void HEPMessage::setIpProtoId(const ::uint8_t& ipProtoId)
{
  _ipProtoId = ipProtoId;
}

inline const boost::asio::ip::address_v4& HEPMessage::getIp4SrcAddress() const
{
  return _ip4SrcAddress;
}

inline void HEPMessage::setIp4SrcAddress(const boost::asio::ip::address_v4& ip4SrcAddress)
{
  _ip4SrcAddress = ip4SrcAddress;
}

inline void HEPMessage::setIp4SrcAddress(const boost::asio::ip::address_v4::bytes_type& ip4SrcAddress)
{
  _ip4SrcAddress = boost::asio::ip::address_v4(ip4SrcAddress);
}

inline void HEPMessage::setIp4SrcAddress(const std::string& ip4SrcAddress)
{
  _ip4SrcAddress = boost::asio::ip::address_v4::from_string(ip4SrcAddress);
}

inline const boost::asio::ip::address_v4& HEPMessage::getIp4DestAddress() const
{
  return _ip4DestAddress;
}

inline void HEPMessage::setIp4DestAddress(const boost::asio::ip::address_v4& ip4DestAddress)
{
  _ip4DestAddress = ip4DestAddress;
}

inline void HEPMessage::setIp4DestAddress(const boost::asio::ip::address_v4::bytes_type& ip4DestAddress)
{
  _ip4DestAddress = boost::asio::ip::address_v4(ip4DestAddress);
}

inline void HEPMessage::setIp4DestAddress(const std::string& ip4DestAddress)
{
  _ip4DestAddress = boost::asio::ip::address_v4::from_string(ip4DestAddress);
}

inline void HEPMessage::setIp6SrcAddress(const boost::asio::ip::address_v6& ip6SrcAddress)
{
  _ip6SrcAddress = ip6SrcAddress;
}

inline void HEPMessage::setIp6SrcAddress(const boost::asio::ip::address_v6::bytes_type& ip6SrcAddress)
{
  _ip6SrcAddress = boost::asio::ip::address_v6(ip6SrcAddress);
}

inline void HEPMessage::setIp6SrcAddress(const std::string& ip6SrcAddress)
{
  _ip6SrcAddress = boost::asio::ip::address_v6::from_string(ip6SrcAddress);
}

inline const boost::asio::ip::address_v6& HEPMessage::getIp6DestAddress() const
{
  return _ip6DestAddress;
}

inline void HEPMessage::setIp6DestAddress(const boost::asio::ip::address_v6& ip6DestAddress)
{
  _ip6DestAddress = ip6DestAddress;
}

inline void HEPMessage::setIp6DestAddress(const boost::asio::ip::address_v6::bytes_type& ip6DestAddress)
{
  _ip6DestAddress = boost::asio::ip::address_v6(ip6DestAddress);
}

inline void HEPMessage::setIp6DestAddress(const std::string& ip6DestAddress)
{
  _ip6DestAddress = boost::asio::ip::address_v6::from_string(ip6DestAddress);
}

inline const ::uint16_t& HEPMessage::getSrcPort() const
{
  return _srcPort;
}

inline void HEPMessage::setSrcPort(const ::uint16_t& srcPort)
{
  _srcPort = srcPort;
}

inline const ::uint16_t& HEPMessage::getDestPort() const
{
  return _destPort;
}

inline void HEPMessage::setDestPort(const ::uint16_t& destPort)
{
  _destPort = destPort;
}

inline const ::uint32_t& HEPMessage::getTimeStamp() const
{
  return _timeStamp;
}

inline void HEPMessage::setTimeStamp(const ::uint32_t& timeStamp)
{
  _timeStamp = timeStamp;
}

inline const ::uint32_t& HEPMessage::getTimeStampMicroOffset() const
{
  return _timeStampMicroOffset;
}

inline void HEPMessage::setTimeStampMicroOffset(const ::uint32_t& timeStampMicroOffset)
{
  _timeStampMicroOffset = timeStampMicroOffset;
}

inline const ::uint8_t& HEPMessage::getProtocolType() const
{
  return _protocolType;
}

inline void HEPMessage::setProtocolType(const ::uint8_t& protocolType)
{
  _protocolType = protocolType;
}

inline const std::string& HEPMessage::getData() const
{
  return _data;
}

inline void HEPMessage::setData(const std::string& data)
{
  _data = data;
}

#endif

