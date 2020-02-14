/**
 * vi:set ts=4 sw=4 expandtab fileencoding=utf-8:
 * @version 2.3.3
 * @author wiley
 * @copyright (C) 2008-2010 D&SOFT
 * @link http://open.coolsms.co.kr
 **/
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include "csmd5.h"
#include "keygen.hpp"
#include "TBSP.hpp"
#include "CSCP.hpp"
#include "coolsms.hpp"
#include "serverloadbalance.hpp"

namespace coolsms {
    /**
     * @brief MD5 encoding
     * @param str string to encode in MD5.
     * @param md5str reference to return MD5 encoded HEX value string.
     */
    void sms::md5get(const std::string str, std::string & md5str)
    {
        unsigned char arr[16];
        char tmp[5];
        unsigned char *input;

        input = (unsigned char *)strdup(str.c_str());

        md5(input, strlen((const char *)input), arr);
        for (int i = 0; i < 16; i++)
        {
            ::sprintf(tmp, "%02x", arr[i]);
            md5str += tmp;
        }

        free(input);
    }

    /**
     * @brief create message-id
     * @return return message-id created.
     */
    std::string sms::keygen()
    {
        return keygen::create();
    }

    void sms::EnableResale()
    {
        is_resale = true;
    }

    void sms::SetSlnRegKey(const std::string & _sln_reg_key)
    {
        sln_reg_key = _sln_reg_key;
        EnableResale();
    }

    /**
     * @brief connect to service servers.
     * @return success true, failure false
     */
    bool sms::connect()
    {
#ifdef _WIN32
        winsock_startup();
#endif

        conn.init();

        return slb::connect(conn, is_resale);
    }

    /**
     * @brief clear added text messages
     */
    void sms::empty(void)
    {
        std::vector<SmsMessagePdu *>::iterator it; 
        
        for (it = smsdata.begin(); it != smsdata.end(); it++) 
        { 
            delete *it; 
        }
        
        smsdata.clear(); 
    }

    /**
     * @brief disconnect
     */
    void sms::disconnect(void)
    {
        conn.close();
#ifdef _WIN32
        winsock_cleanup();
#endif
    }

    /**
     * @brief enqueue a text message.
     *
     * @param called_number recipient number
     * @param calling_number sender number
     * @param message content
     * @param called_refname reference name, default ""
     * @param reservdate reservation date (format YYYYMMDDHHMISS), default "" (send immediately)
     * @param message_id local message-id, default = "" (create by server)
     */
    void sms::add(std::string called_number
        , std::string calling_number
        , std::string message
        , std::string called_refname
        , std::string reservdate
        , std::string message_id
        , std::string type
        , std::string subject
        )
    {
        SmsMessagePdu *smsunit; 
        
        smsunit = new SmsMessagePdu; 
        smsunit->called_number = called_number;
        smsunit->calling_number = calling_number;
        smsunit->message = message;
        smsunit->called_refname = called_refname;
        smsunit->reservdate = reservdate;
        smsunit->message_id = message_id; 
        smsunit->type = type; 
        smsunit->subject = subject; 
        
        smsdata.push_back(smsunit); 
    }

    /**
     * @brief get balance information.
     * @param SmsBalancePdu
     * @return true : success, falses : failure
     */
    bool sms::getBalance(SmsBalancePdu & sbp)
    {
        std::map<std::string,std::string> tbspmap;
        std::string passwd;

        tbspmap["VERSION"] = "TBSP/1.0";
        tbspmap["COMMAND"] = "CHECK-REMAIN";
        tbspmap["MODULE-VERSION"] = module_version;
        if (app_version.size()) tbspmap["APP-VERSION"] = app_version;
        if (_charset.size()) tbspmap["CHARSET"] = _charset;
        if (this->crypt.size()) tbspmap["CRYPT-METHOD"] = this->crypt;
        if (this->passwd.size()) tbspmap["AUTH-PASS"] = this->passwd;
        if (this->userid.size()) tbspmap["AUTH-ID"] = this->userid;
        tbspmap["LANGUAGE"] = "C++";
        std::map<std::string,std::string>::iterator it;
        for (it = extraFields.begin(); it != extraFields.end(); it++)
        {
            if (it->first.size() && it->second.size()) tbspmap["X-"+it->first] = it->second;
        }

        std::string tbsp_str;
        std::string cscp_str;
        TBSP::build(tbspmap, tbsp_str);

        CSCP cscp;
        cscp.addparam(CSCPPARAM_ID_MESSAGE, tbsp_str);
        cscp.build(cscp_str);

        if (conn.send(cscp_str) <= 0)
        {
            error_string = "tcp send error.";
            return false;
        }

        CSCP ack;
        std::string cscp_rstr;
        if (!ack.read(conn))
        {
            error_string = "CSCP read error.";
            return false;
        }
        CSCPPARAM *param = ack.getparam(CSCPPARAM_ID_MESSAGE);
        cscp_rstr = param->getBody();


        TBSP tbsp;

        if (!tbsp.parse(cscp_rstr))
        {
            error_string = "TBSP parse error.";
            return false;
        }

        std::string remain_str;
        std::string cash_str;
        std::string point_str;
        std::string drop_str;

        tbsp.getval("RESULT-CODE", sbp.resultCode);
        tbsp.getval("RESULT-MESSAGE", sbp.resultMessage);
        tbsp.getval("CASH", sbp.cash);
        tbsp.getval("POINT", sbp.point);
        tbsp.getval("DROP", sbp.mdrop);
        tbsp.getval("CREDITS", sbp.smsBalance);

        if (sbp.resultCode == "00")
        {
            // success
        } else 	if (sbp.resultCode == "30") {
            error_string = "No balance.";
            return false;
        } else 	if (sbp.resultCode == "20") {
            error_string = "Non exist user id or wrong password.";
            return false;
        } else {
            // etc.
            error_string = "Error Code: " + sbp.resultCode;
            return false;
        }

        return true;
    }

    /**
     * @brief get balance (cash, point, mdrops)
     * @param cash Cash
     * @param point Point
     * @param drop M-drops
     * @param remain calculated SMS balance(cash + point + mdrops)
     * @param resultcode result code(00 = success)
     * @return true : success, falses : failure
     */
    bool sms::remain(long & cash, long & point, long & drop, long & remain, std::string & resultcode)
    {
        SmsBalancePdu sbp;
        bool res = getBalance(sbp);
        cash = sbp.cash;
        point = sbp.point;
        drop = sbp.mdrop;
        remain = sbp.smsBalance;
        resultcode = sbp.resultCode;
        return res;
    }

    std::string sms::basename(const std::string & str)
    {
        size_t found;
        found = str.find_last_of("/\\");
        if (found == str.npos) return str;
        //str.substr(0, found); // folder
        return str.substr(found+1);
    }

    bool sms::file_get_contents(const std::string & fileName, void **buf, long * len)
    {
        FILE *fp;
        std::string file;
        file = basename(fileName);
        std::cout << "file : " << file << std::endl;
        fp=fopen(fileName.c_str(),"rb");
        if (!fp) return false;
        fseek(fp,0,SEEK_END); //go to end
        long content_length=ftell(fp); //get position at end (length)
        *len = content_length + file.size() + 3;
        fseek(fp,0,SEEK_SET); //go to beg.
        *buf=malloc(*len); //malloc buffer
        if (!*buf) return false;

        std::cout << "content-length : " << content_length << std::endl;
        std::cout << "len : " << *len << std::endl;

        char *p = (char *)*buf;

        // put filename size
        char tmp[80];
        sprintf(tmp, "%03u", (int)file.size());
        std::cout << "file.size: " << tmp << std::endl;
        std::cout << "file.name: " << file << std::endl;
        memcpy(p, tmp, 3);
        p += 3;
        // put filename
        memcpy(p, file.c_str(), file.size());
        p += file.size();
        // put contents
        fread(p,content_length,1,fp); //read into buffer
        fclose(fp);

        return true;
    }
    

    /**
     * @return -1: error, others: request count
     */
    int sms::send()
    {
        std::vector<SmsMessagePdu *>::iterator it;
        SmsMessagePdu *smsunit;
        std::string passwd;


        int count=0;
        for (it = smsdata.begin(); it != smsdata.end(); it++)
        {
            smsunit = *it;

            std::map<std::string,std::string> tbspmap;

            if (is_resale) tbspmap["SLN-REG-KEY"] = sln_reg_key;
            tbspmap["VERSION"] = "TBSP/1.0";
            tbspmap["COMMAND"] = "SEND";
            tbspmap["MODULE-VERSION"] = module_version;
            if (app_version.size()) tbspmap["APP-VERSION"] = app_version;
            if (_charset.size()) tbspmap["CHARSET"] = _charset;
            if (smsunit->message.size()) tbspmap["MESSAGE"] = smsunit->message;
            if (smsunit->message_id.size()) tbspmap["MESSAGE-ID"] = smsunit->message_id;
            if (smsunit->groupId.size()) tbspmap["GROUP-ID"] = smsunit->groupId;
            if (smsunit->called_number.size()) tbspmap["CALLED-NUMBER"] = smsunit->called_number;
            if (smsunit->calling_number.size()) tbspmap["CALLING-NUMBER"] = smsunit->calling_number;
            if (smsunit->called_refname.size()) tbspmap["CALLED-NAME"] = smsunit->called_refname;
            if (smsunit->reservdate.size()) tbspmap["RESERVE-DATE"] = smsunit->reservdate;
            if (this->crypt.size()) tbspmap["CRYPT-METHOD"] = this->crypt;
            if (this->userid.size()) tbspmap["AUTH-ID"] = this->userid;
            if (this->passwd.size()) tbspmap["AUTH-PASS"] = this->passwd;
            if (smsunit->type.size()) tbspmap["TYPE"] = smsunit->type;
            if (smsunit->subject.size()) tbspmap["SUBJECT"] = smsunit->subject;
			if (smsunit->delayCount) {
				std::stringstream ss;
				ss << smsunit->delayCount;
				tbspmap["DELAY-COUNT"] = ss.str();
			}
            tbspmap["LANGUAGE"] = "C++";
			std::map<std::string,std::string>::iterator it;
            for (it = extraFields.begin(); it != extraFields.end(); it++)
            {
                if (it->first.size() && it->second.size()) tbspmap["X-"+it->first] = it->second;
            }


            CSCP cscp;
            if (smsunit->imageFile.size()) {
                tbspmap["ATTACHMENT"] = basename(smsunit->imageFile);
                void *binary=0;
                long len=0;
                file_get_contents(smsunit->imageFile, &binary, &len);
                std::cout << "param body len: " << len << std::endl;
                if (len > 0) cscp.addparam(CSCPPARAM_ID_ATTACHMENT, binary, len);
            }

            std::string tbsp_str;
            void *cscpdata=0;
            long len=0;
            TBSP::build(tbspmap, tbsp_str);
            cscp.addparam(CSCPPARAM_ID_MESSAGE, tbsp_str);
            cscp.build(&cscpdata, &len);

            if (!conn.send(cscpdata, len))
            {
                error_string = "tcp send error.";
                return -1;
            }

            std::string cscp_rstr;
            CSCP ack;
            ack.read(conn);
            CSCPPARAM * param = ack.getparam(CSCPPARAM_ID_MESSAGE);
            cscp_rstr = param->getBody();

            TBSP tbsp;
            if (!tbsp.parse(cscp_rstr))
            {
                error_string = "TBSP parse error.";
                continue;
            }
            tbsp.getval("RESULT-CODE", smsunit->reportResultCode);
            tbsp.getval("RESULT-MESSAGE", smsunit->reportResultMessage);
            tbsp.getval("MESSAGE-ID", smsunit->reportMessageId);
            tbsp.getval("GROUP-ID", smsunit->reportGroupId);

            if (smsunit->reportResultCode == "00")
                count++;
        }

        return count;
    }

    /**
     * @brief check result value
     * @param messageId message-id
     * @param status 0: waitting, 1: sending, 2: complete
     * @param resultcode 00 : normal(success) , 10 : wrong number, 20 : wrong id or wrong password, 30 : no balance
     */
    bool sms::rcheck(const std::string & messageId, std::string & status, std::string & resultcode)
    {
        std::map<std::string,std::string> tbspmap;
        std::string passwd;

        tbspmap["VERSION"] = "TBSP/1.0";
        tbspmap["COMMAND"] = "CHECK-RESULT";
        tbspmap["MODULE-VERSION"] = module_version;
        if (app_version.size()) tbspmap["APP-VERSION"] = app_version;
        if (_charset.size()) tbspmap["CHARSET"] = _charset;
        if (this->crypt.size()) tbspmap["CRYPT-METHOD"] = this->crypt;
        if (this->userid.size()) tbspmap["AUTH-ID"] = this->userid;
        if (this->passwd.size()) tbspmap["AUTH-PASS"] = this->passwd;
        if (messageId.size()) tbspmap["MESSAGE-ID"] = messageId;
        tbspmap["LANGUAGE"] = "C++";

        std::string tbsp_str;
        void *cscpdata=0;
        long len=0;
        TBSP::build(tbspmap, tbsp_str);

        CSCP cscp;
        cscp.addparam(CSCPPARAM_ID_MESSAGE, tbsp_str);
        cscp.build(&cscpdata, &len);

        conn.send(cscpdata, len);

        std::string cscp_rstr;
        CSCP ack;
        ack.read(conn);
        CSCPPARAM * param = ack.getparam(CSCPPARAM_ID_MESSAGE);
        cscp_rstr = param->getBody();

        TBSP tbsp;
        if (!tbsp.parse(cscp_rstr))
        {
            error_string = "TBSP parse error.";
            return false;
        }
        tbsp.getval("STATUS", status);
        tbsp.getval("RESULT-CODE", resultcode);

        return true;
    }

    /**
     * @brief cancelation
     * @param messageId message-id
     * @param resultcode 00 : normal(success) , 10 : wrong number, 20 : wrong id or password, 30 : no balance
     */
    bool sms::cancel(const std::string & messageId, std::string & resultCode, std::string & resultMessage)
    {
        std::map<std::string,std::string> tbspmap;
        std::string passwd;

        tbspmap["VERSION"] = "TBSP/1.0";
        tbspmap["COMMAND"] = "CANCEL";
        if (messageId.size()) tbspmap["MESSAGE-ID"] = messageId;
        tbspmap["MODULE-VERSION"] = module_version;
        if (app_version.size()) tbspmap["APP-VERSION"] = app_version;
        if (_charset.size()) tbspmap["CHARSET"] = _charset;
        if (this->crypt.size()) tbspmap["CRYPT-METHOD"] = this->crypt;
        if (this->userid.size()) tbspmap["AUTH-ID"] = this->userid;
        if (this->passwd.size()) tbspmap["AUTH-PASS"] = this->passwd;
        tbspmap["LANGUAGE"] = "C++";

        std::string tbsp_str;
        std::string cscp_str;
        TBSP::build(tbspmap, tbsp_str);

        CSCP cscp;
        cscp.addparam(CSCPPARAM_ID_MESSAGE, tbsp_str);
        cscp.build(cscp_str);

        conn.send(cscp_str);

        std::string cscp_rstr;
        CSCP ack;
        ack.read(conn);
        CSCPPARAM * param = ack.getparam(CSCPPARAM_ID_MESSAGE);
        cscp_rstr = param->getBody();

        TBSP tbsp;
        if (!tbsp.parse(cscp_rstr))
        {
            error_string = "TBSP parse error.";
            return false;
        }
        tbsp.getval("RESULT-CODE", resultCode);
        tbsp.getval("RESULT-MESSAGE", resultMessage);

        return true;
    }

    /**
     * @brief group cancenlation
     * @param msgid message-id
     * @param resultcode 00 : normal(success) , 10 : wrong number, 20 : wrong id or password, 30 : no balance
     */
    bool sms::groupcancel(const std::string & group_id, std::string & resultCode, std::string & resultMessage)
    {
        std::map<std::string,std::string> tbspmap;
        std::string passwd;

        tbspmap["VERSION"] = "TBSP/1.0";
        tbspmap["COMMAND"] = "GROUP-CANCEL";
        if (group_id.size()) tbspmap["GROUP-ID"] = group_id;
        tbspmap["MODULE-VERSION"] = module_version;
        if (app_version.size()) tbspmap["APP-VERSION"] = app_version;
        if (_charset.size()) tbspmap["CHARSET"] = _charset;
        if (this->crypt.size()) tbspmap["CRYPT-METHOD"] = this->crypt;
        if (this->userid.size()) tbspmap["AUTH-ID"] = this->userid;
        if (this->passwd.size()) tbspmap["AUTH-PASS"] = this->passwd;
        tbspmap["LANGUAGE"] = "C++";

        std::string tbsp_str;
        TBSP::build(tbspmap, tbsp_str);

        CSCP cscp;
        void *cscp_data=0;
        long len=0;
        cscp.addparam(CSCPPARAM_ID_MESSAGE, tbsp_str);
        cscp.build(&cscp_data, &len);

        conn.send(cscp_data, len);

        std::string cscp_rstr;
        CSCP ack;
        ack.read(conn);
        CSCPPARAM * param = ack.getparam(CSCPPARAM_ID_MESSAGE);
        cscp_rstr = param->getBody();

        TBSP tbsp;
        if (!tbsp.parse(cscp_rstr))
        {
            error_string = "TBSP parse error.";
            return false;
        }
        tbsp.getval("RESULT-CODE", resultCode);
        tbsp.getval("RESULT-MESSAGE", resultMessage);

        return true;
    }

     /**
     * @brief set user-id, password
     * @param userid user-id
     * @param passwd password
     * @param crypt crypt algorithm
     */
    void sms::setuser(const std::string & userid, const std::string & passwd, const std::string & crypt)
    {
   
        this->userid = userid;
        if (crypt == "MD5" || crypt == "md5")
        {
            this->passwd = "";
            md5get(passwd, this->passwd);
        }
        else
        {
            this->passwd = passwd;
        }
    }

    /**
     * @brief last error string
     * @return error string
     */
    std::string sms::lasterrorstr()
    {
        return error_string;
    }

    bool sms::errordetected()
    {
        if (error_string.length() > 0) return true;
        return false;
    }

    void sms::printr(void)
    {
        std::vector<SmsMessagePdu *>::iterator it;
        SmsMessagePdu *smsunit;

        for (it = smsdata.begin(); it != smsdata.end(); it++)
        {
            smsunit = *it;
            std::cout << "MessageId: " << smsunit->reportMessageId << std::endl;
            std::cout << "DestinationAddress: " << smsunit->called_number << std::endl;
            std::cout << "SourceAddress: " << smsunit->calling_number << std::endl;
            std::cout << "GroupId: " << smsunit->reportGroupId << std::endl;
            std::cout << "ResultCode: " << smsunit->reportResultCode << std::endl;
            std::cout << "ResultMessage: " << smsunit->reportResultMessage << std::endl;
        }
    }
}
