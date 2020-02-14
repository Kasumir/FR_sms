/**
 * vi:set ts=4 sw=4 expandtab fileencoding=utf-8:
 * @author wiley
 * @copyright (C) 2008-2010 D&SOFT
 * @link http://open.coolsms.co.kr
 **/
#ifndef __coolsms_h__
#define __coolsms_h__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include "tcpconn.hpp"

namespace coolsms {
    struct smsunit_struct
    {
        smsunit_struct() : called_number()
                           , calling_number()
                           , message()
                           , called_refname()
                           , reservdate()
                           , message_id()
                           , groupId()
                           , type()
                           , subject()
                           , imageFile()
                           , delayCount(0) 
                           , reportMessageId()
                           , reportGroupId()
                           , reportResultCode()
                           , reportResultMessage()
        {}
        std::string called_number;
        std::string calling_number;
        std::string message;
        std::string called_refname;
        std::string reservdate;
        std::string message_id;
        std::string groupId;
        std::string type;
        std::string subject;
        std::string imageFile;
        int delayCount;
        // report
        std::string reportMessageId;
        std::string reportGroupId;
        std::string reportResultCode;
        std::string reportResultMessage;
    };
    typedef struct smsunit_struct SmsMessagePdu;

    struct smsbalance_struct
    {
        smsbalance_struct() : cash(0)
                              , point(0)
                              , mdrop(0)
                              , smsBalance(0)
                              , resultCode()
                              , resultMessage()
        {}
        long cash;
        long point;
        long mdrop;
        long smsBalance;
        std::string resultCode;
        std::string resultMessage;
    };
    typedef struct smsbalance_struct SmsBalancePdu;

    class sms
    {
        public:
            sms() : smsdata() 
            , userid() 
            , passwd() 
            , conn() 
            , crypt("MD5") 
            , error_string()
            , module_version("C++/2.3.2")
            , app_version("")
            , _charset("euckr")
            , extraFields()
            , sln_reg_key("")
            , is_resale(false)
            {
            }
            void md5get(const std::string str, std::string & md5str);
            std::string keygen();
            bool connect();
            void empty();
            void clear() { empty(); }
            void disconnect();
            void close() { disconnect(); empty(); }
            void add(SmsMessagePdu * pdu) { smsdata.push_back(pdu); }
            void add(std::string called_number
                , std::string calling_number
                , std::string message
                , std::string called_refname = ""
                , std::string reservdate = ""
                , std::string message_id = ""
                , std::string type = "SMS"
                , std::string subject = ""
                );
            void addsms(std::string called_number
                , std::string calling_number
                , std::string message
                , std::string called_refname = ""
                , std::string reservdate = ""
                , std::string message_id = ""
                )
            {
                add(called_number, calling_number, message, called_refname, reservdate, message_id, "SMS");
            }
            void addlms(std::string called_number
                , std::string calling_number
                , std::string subject
                , std::string message
                , std::string called_refname = ""
                , std::string reservdate = ""
                , std::string message_id = ""
                )
            {
                add(called_number, calling_number, message, called_refname, reservdate, message_id, "LMS", subject);
            }

            bool getBalance(SmsBalancePdu & sbp);
            bool remain(long & cash, long & point, long & drop, long & remain, std::string & resultcode);
            int send();
            bool rcheck(const std::string & mid, std::string & status, std::string & result_code);
            void setCrypt(const std::string & crypt) { this->crypt = crypt; }
            void setuser(const std::string & userid, const std::string & passwd, const std::string & crypt);
            void setuser(const std::string & userid, const std::string & passwd) { setuser(userid, passwd, "MD5"); }
            std::string lasterrorstr();
            bool errordetected();
            void charset(const std::string & _charset) { this->_charset = _charset; }
            void appversion(const std::string & version) { app_version = version; }
            bool cancel(const std::string & msgid, std::string & resultCode, std::string & resultMessage);
            bool groupcancel(const std::string & group_id, std::string & resultCode, std::string & resultMessage);
            bool setcallbackurl(const std::string & callback_url, std::string & status, std::string & resultcode);
            void printr();
            std::string basename(const std::string & str);
            bool file_get_contents(const std::string & fileName, void **buf, long * len);
            void addExtraField(const std::string & fieldName, const std::string & fieldValue) { extraFields[fieldName] = fieldValue; }
            void EnableResale();
            void SetSlnRegKey(const std::string & _sln_reg_key);

        public:
            std::vector<struct smsunit_struct *> smsdata;

        private:
            std::string userid;
            std::string passwd;
            tcpconn conn;
            std::string crypt;
            std::string error_string;
            std::string module_version;
            std::string app_version;
            std::string _charset;
            std::map<std::string,std::string> extraFields;
            std::string sln_reg_key;
            bool is_resale;
    };
}

#endif /*  __coolsms_h__ */
