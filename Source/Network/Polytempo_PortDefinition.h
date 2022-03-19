#pragma once

#define POLYTEMPO_NETWORK_PORT_SERVER   47521
#define POLYTEMPO_NETWORK_PORT_APP      47522
#define POLYTEMPO_NETWORK_PORT_MAX      47523
#define POLYTEMPO_IPC_PORT              47524

const int Polytempo_AdvertisePortCount = 3;
const int Polytempo_AdvertisePorts[Polytempo_AdvertisePortCount] = {
    POLYTEMPO_NETWORK_PORT_SERVER,
    POLYTEMPO_NETWORK_PORT_APP,
    POLYTEMPO_NETWORK_PORT_MAX
};
