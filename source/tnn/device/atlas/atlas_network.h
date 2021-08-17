// Copyright 2019 Tencent. All Rights Reserved

#ifndef TNN_SOURCE_DEVICE_ATLAS_ATLAS_NETWORK_H_
#define TNN_SOURCE_DEVICE_ATLAS_ATLAS_NETWORK_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "acl/acl.h"
#include "tnn/core/abstract_network.h"
#include "tnn/core/macro.h"
#include "tnn/device/atlas/atlas_common_types.h"

namespace TNN_NS {

class AtlasNetwork : public AbstractNetwork {
public:
    // @brief virtual default destructor
    virtual ~AtlasNetwork();

    // @brief init network with net cfg and net res.
    // @param net_cfg
    // @param net_res
    virtual Status Init(NetworkConfig &net_config, ModelConfig &model_config, AbstractModelInterpreter *interpreter,
                        InputShapesMap min_inputs_shape, InputShapesMap max_inputs_shape,
                        bool enable_const_folder = true);

    // @brief deinit release init create resource
    virtual Status DeInit();

    //  @brief return the amount of memory required for forward
    //  @param memory_size: the memory size used by tnn layers for
    //  forward
    //  @return error code: If successful, returns zero. Otherwise, returns
    //  an error code.
    virtual Status GetForwardMemorySize(int &memory_size);

    //  @brief: set memory used by the tnn instance without forward
    //  memory, the memory size must be at least that returned by
    //  GetForwardMemorySize(). releasing or otherwise using the memory for
    //  other purposes during the tnn network run will result in
    //  undefined behavior.
    //  @param memory: the memory used by tnn layers for forward
    //  @return error code: If successful, returns zero. Otherwise, returns
    //  an error code.
    //
    virtual Status SetForwardMemory(void *memory);

    // @brief network infer
    virtual Status Reshape(const InputShapesMap &inputs);

    // @brief get tnn command queue
    // @param command_queue device command queue for forward
    virtual Status GetCommandQueue(void **command_queue);

    // @brief network infer, it will sync to wait result
    virtual Status Forward();

    // @brief tnn instance network infer, it will not wait
    virtual Status ForwardAsync(Callback call_back);

    // @brief get all input blobs
    // @param blobs input blobs name map
    virtual Status GetAllInputBlobs(BlobMap &blobs);

    // @brief get all output blobs
    // @param blobs output blobs name map
    virtual Status GetAllOutputBlobs(BlobMap &blobs);

private:
    // @brief load model from om file
    Status LoadModelFromFile(const std::string &om_file);

    // @brief load model from memory
    Status LoadModelFromMemory(const std::string &om_file);

    // @brief unload model
    void UnloadModel();

    // @brief allocate data set
    Status AllocateDataset(aclmdlDataset **data_set, bool is_input);

    // @brief add blob into map
    Status AddBlobToMap(size_t index, void *data, bool is_input);

    // @brief get input dims
    Status GetInputInfo(size_t index, std::vector<int> &input_dims, aclFormat &input_format,
                        aclDataType &input_data_type);

    // @brief set dynmaic batch size
    Status SetDynamicBatchSize(std::string blob_name, int batch_size);

    // @brief destory dataset
    void DestroyDataset(aclmdlDataset *&data_set);

    BlobMap input_blob_map_;
    BlobMap output_blob_map_;

    bool need_to_deinit                               = false;
    std::shared_ptr<AtlasCommandQueue> command_queue_ = nullptr;
    aclrtContext context_                             = nullptr;
    aclrtStream stream_                               = nullptr;
    size_t model_mem_size_                            = 0;
    size_t model_weight_size_                         = 0;
    void *model_mem_ptr_                              = nullptr;
    void *model_weight_ptr_                           = nullptr;
    uint32_t model_id_                                = 0;
    aclmdlDesc *model_desc_                           = nullptr;
    aclmdlDataset *input_                             = nullptr;
    aclmdlDataset *output_                            = nullptr;
    std::vector<std::string> dynamic_batch_name_;
    bool has_aipp_ = false;
    std::map<std::string, aclAippInputFormat> aipp_input_format_map_;
    std::map<std::string, int> output_dim0_map_;
};

}  // namespace TNN_NS

#endif  // TNN_SOURCE_DEVICE_ATLAS_ATLAS_NETWORK_H_