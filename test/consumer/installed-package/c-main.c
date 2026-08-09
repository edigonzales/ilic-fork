#include "ilic/capi.h"

int main(void)
{
    return ilic_abi_version() == 0 ? 1 : 0;
}
