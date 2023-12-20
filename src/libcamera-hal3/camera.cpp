class Camera: public Object {
public:
    Camera(camera_module_t *camera_module, int camera_id)
    : cameraModule(camera_module),
      cameraId(camera_id),
      cameraDevice(nullptr)
    {
        // Initialize Camera HAL3
    }

    ~Camera();
    const std::string &id() const;
    int acquire();
    int release();
    const ControlInfoMap &controls() const;
    const ControlList &properties() const;
    const std::set<Stream *> &streams() const;
    std::unique_ptr<CameraConfiguration> generateConfiguration(Span<const StreamRole> roles = {});
    int configure(CameraConfiguration *config);
    std::unique_ptr<Request> createRequest(uint64_t cookie = 0);
    
    int queueRequest(Request *request);
    int start(const ControlList *controls = nullptr);
    int stop();

private:
    camera_module_t* cameraModule;
    int cameraId;
    camera3_device_t* cameraDevice;
};

int Camera::acquire() 
{
    // Create a hardware device and get its corresponding hw_device_t instance.
    hw_device_t *device = nullptr;
    int result = cameraModule->common.methods->open(const_cast<hw_module_t *>
                                                    (reinterpret_cast<const hw_module_t *>(cameraModule)),
                                                    std::to_string(cameraId).c_str(),
                                                    &device);
    if (result != 0) {
        // Error handling: failed to open the device.
        return -1;
    }

    // Confirm this is a version CAMERA_DEVICE_API_VERSION_3_0 or above device.
    if (device->version < CAMERA_DEVICE_API_VERSION_3_0) {
        // Error handling: wrong device version.
        return -2;
    }

    // Now that we've confirmed it's a camera3 device, convert device to the camera3_device structure.
    cameraDevice = reinterpret_cast<camera3_device_t *>(device);

    // More settings and initialization routines here, such as setting up camera3_callback_ops_t...

    return 0;  // Success!
}

int Camera::release() 
{
    // First, stop any ongoing operations
    // handle stopping...

    // Then, clear any pending requests
    // handle clearing requests...

    // Now, check if the camera device exists and close it
    if (cameraDevice) {
        cameraDevice->common.close(&cameraDevice->common);
        cameraDevice = nullptr;
    }

    return 0;  // Success!
}

int Camera::configure(CameraConfiguration *config)
{
    if (!cameraDevice) {
        // Error: camera device doesn't exist
        return -1;
    }

    // Create camera3_stream_configuration_t and populate with initial settings...
    camera3_stream_configuration_t stream_config {
        .streams = nullptr,        // Array of pointers to streams
        .num_streams = 0,          // Number of streams in above stream array
        .operation_mode = 0,       // Operation mode of streams
    };

    // Populate stream_config based on CameraConfiguration
    // ... 

    int result = cameraDevice->ops->configure_streams(cameraDevice, &stream_config);
    if (result != 0) {
        // Error: failed to configure streams
        return -2;
    }

    return 0;  // Success!
}

int Camera::queueRequest(CameraRequest *request) 
{
    if (!cameraDevice) {
        // Error: camera device doesn't exist
        return -1;
    }

    // Initialize camera3_capture_request object.
    // Buffers, count of input and output buffers, settings, etc need to be provided.

    camera3_capture_request_t capture_request = {
        .frame_number = 0,    // Placeholder. Should be unique for each request.
        .settings = nullptr,  // Placeholder. Should be derived from CameraRequest object.
        .input_buffer = nullptr,  // Placeholder. nullptr for capture requests.
        .num_output_buffers = 0,  // Placeholder. Should be set according to CameraRequest object.
        .output_buffers = nullptr  // Placeholder. Should be created from CameraRequest object.
    };

    // Update capture_request fields based on CameraRequest.
    // ...

    // Send the configured request to HAL.
    int result = cameraDevice->ops->process_capture_request(cameraDevice, &capture_request);
    if (result != 0) {
        // Error: failed to queue request
        return -2;
    }

    return 0;  // Success!
}

//Camera::start
int Camera::start()
{
    if (!cameraDevice) {
        // Error: Camera device doesn't exist
        return -1;
    }

    // Assuming a repeating request instance is already available
    // Make sure to include appropriate checks and method calls in your real scenario

    // The queueRequest method를 use the request as parameter.
    // Here, 'repeatingRequest' is an instance of 'Request'.
    int result = queueRequest(repeatingRequest.get());

    if (result != 0) {
        // Error: Failed to start the camera
        return -2;
    }

    return 0;  // Success!
}

//Camera::stop
int Camera::stop()
{
    if (!cameraDevice) {
        // Error: Camera device doesn't exist
        return -1;
    }

    // Assuming the stop operation corresponds to a flush() call on camera_device_ops_t
    // It instructs device to cancel all pending requests, regardless of target.

    int result = cameraDevice->ops->flush(cameraDevice);
    if (result != 0) {
        // Error: Failed to stop the camera
        return -2;
    }

    return 0;  // Success!
}