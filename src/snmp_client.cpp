#include "snmp_client.hpp"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <cstdlib>
#include <string>

namespace {
std::string variableToString(netsnmp_variable_list* variable) {
    if (!variable) return {};
    char buffer[2048]{};
    snprint_value(buffer, sizeof(buffer), variable->name, variable->name_length, variable);
    return std::string(buffer);
}

std::string getScalar(netsnmp_session* session, const char* oidText) {
    oid objectId[MAX_OID_LEN];
    size_t objectIdLength = MAX_OID_LEN;
    if (!read_objid(oidText, objectId, &objectIdLength)) return {};

    netsnmp_pdu* pdu = snmp_pdu_create(SNMP_MSG_GET);
    snmp_add_null_var(pdu, objectId, objectIdLength);

    netsnmp_pdu* response = nullptr;
    const int status = snmp_synch_response(session, pdu, &response);

    std::string result;
    if (status == STAT_SUCCESS && response &&
        response->errstat == SNMP_ERR_NOERROR && response->variables) {
        result = variableToString(response->variables);
    }
    if (response) snmp_free_pdu(response);
    return result;
}
}

SnmpClient::SnmpClient(const std::string& community, int timeoutMs, int retries)
    : community_(community), timeoutMs_(timeoutMs), retries_(retries) {
    init_snmp("nms-discovery");
}

SnmpClient::~SnmpClient() = default;

Device SnmpClient::discover(const std::string& ip) {
    Device device;
    device.ip = ip;

    netsnmp_session session{};
    snmp_sess_init(&session);
    session.version = SNMP_VERSION_2c;
    session.peername = strdup(ip.c_str());
    session.community = reinterpret_cast<u_char*>(const_cast<char*>(community_.c_str()));
    session.community_len = community_.size();
    session.timeout = static_cast<long>(timeoutMs_) * 1000L;
    session.retries = retries_;

    netsnmp_session* opened = snmp_open(&session);
    if (!opened) {
        device.error = "Unable to open SNMP session";
        if (session.peername) free(session.peername);
        return device;
    }

    device.sysDescr = getScalar(opened, "1.3.6.1.2.1.1.1.0");
    device.sysObjectId = getScalar(opened, "1.3.6.1.2.1.1.2.0");
    device.sysUpTime = getScalar(opened, "1.3.6.1.2.1.1.3.0");
    device.sysName = getScalar(opened, "1.3.6.1.2.1.1.5.0");

    device.reachable = !device.sysName.empty() ||
                       !device.sysDescr.empty() ||
                       !device.sysObjectId.empty();

    if (!device.reachable) {
        device.error = "No valid SNMP response";
        snmp_close(opened);
        if (session.peername) free(session.peername);
        return device;
    }

    const std::string desc = device.sysDescr;
    if (desc.find("router") != std::string::npos ||
        desc.find("routing") != std::string::npos)
        device.deviceType = "router";
    else if (desc.find("switch") != std::string::npos)
        device.deviceType = "switch";
    else if (desc.find("printer") != std::string::npos)
        device.deviceType = "printer";
    else if (desc.find("Linux") != std::string::npos ||
             desc.find("linux") != std::string::npos)
        device.deviceType = "linux-host";
    else if (desc.find("Windows") != std::string::npos ||
             desc.find("windows") != std::string::npos)
        device.deviceType = "windows-host";
    else
        device.deviceType = "unknown";

    snmp_close(opened);
    if (session.peername) free(session.peername);
    return device;
}
