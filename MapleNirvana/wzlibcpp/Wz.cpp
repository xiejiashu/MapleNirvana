#include "Wz.hpp"
#include "Property.hpp"

u32 wz::get_version_hash(i32 encryptedVersion, i32 realVersion) {
    i32 versionHash = 0;
    auto versionString = std::to_string(realVersion);

    auto len = versionString.size();

    for (int i = 0; i < len; ++i) {
        versionHash = (32 * versionHash) + static_cast<i32>(versionString[i]) + 1;
    }

#define HASHING(V, S) ((V >> S##u) & 0xFFu)
#define AUTO_HASH(V) (0xFFu ^ HASHING(V, 24) ^ HASHING(V, 16) ^ HASHING(V, 8) ^ V & 0xFFu)

    i32 decryptedVersionNumber = AUTO_HASH(static_cast<u32>(versionHash));

    if (encryptedVersion == decryptedVersionNumber) {
        return static_cast<u32>(versionHash);
    }

    return 0;
}
