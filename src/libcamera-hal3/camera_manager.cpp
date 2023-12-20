#include "camera.h"
#include <hardware/hardware.h>
#include <hardware/camera3.h>

namespace libcamera {

class CameraManager : public Object, public Extensible
{
public:
    CameraManager()
    {
        hw_module_t* module;
        int errorCode = hw_get_module(CAMERA_HARDWARE_MODULE_ID, (const hw_module_t**)&module);

        camera_module_ = reinterpret_cast<camera_module_t*>(module);
        if (errorCode == 0) {
            int cameraCount = camera_module_->get_number_of_cameras();
            for (int cameraId = 0; cameraId < cameraCount; cameraId++) {
                cameras_.push_back(std::make_shared<Camera>(camera_module_, cameraId));
            }
        }
    }

    ~CameraManager() { }

    int start()
    {
        for (auto &camera : cameras_)
            if (camera->start() != 0)
                return -1;
        return 0;
    }

    void stop()
    {
        for (auto &camera : cameras_)
            camera->stop();
    }

    std::vector<std::shared_ptr<Camera>> cameras() const
    {
        return cameras_;
    }

    std::shared_ptr<Camera> getCamera(const std::string &cameraId)
    {
        // Find the camera with the specified ID
        for (auto const& camera : cameras_) {
            if (camera->getId() == cameraId) {
                return camera;
            }
        }
        return nullptr;   // If the desired camera is not found, return nullptr
    }

private:
    std::vector<std::shared_ptr<Camera>> cameras_;
    camera_module_t* camera_module_;
};

} // namespace libcamera