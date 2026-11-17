// SPDX-FileCopyrightText: The LineageOS Project
// SPDX-License-Identifier: BSD-3-Clause

#include "dsp_service_client.h"

#include <aidl/vendor/qti/hardware/dsp/IDspService.h>
#include <android/binder_manager.h>
#include <errno.h>

#include <memory>
#include <string>

using ::aidl::vendor::qti::hardware::dsp::IDspService;

int open_device_node_from_service(int domain) {
  const std::string instance = std::string(IDspService::descriptor) + "/default";
  if (!AServiceManager_isDeclared(instance.c_str())) {
    errno = EACCES;
    return -1;
  }

  std::shared_ptr<IDspService> service = IDspService::fromBinder(
      ndk::SpAIBinder(AServiceManager_waitForService(instance.c_str())));
  if (service == nullptr) {
    errno = EACCES;
    return -1;
  }

  ndk::ScopedFileDescriptor node;
  ndk::ScopedAStatus status = service->openSession(domain, &node);
  if (!status.isOk()) {
    int error = status.getServiceSpecificError();
    errno = error > 0 ? error : EACCES;
    return -1;
  }

  return node.release();
}
