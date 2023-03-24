#include <memory.h>
#include <unistd.h>
#include <curl/curl.h>
#include <stdlib.h>
#include "asrmain.h"
#include <string.h>

/**
 * @brief 请填写这个函数开头的参数
 * @param config
 */
RETURN_CODE fill_config(struct asr_config *config,char *file) {
    // 填写网页上申请的appkey 如 g_api_key="g8eBUMSokVB1BHGmgxxxxxx"
    char api_key[] = "填写网页上申请的API key";
    // 填写网页上申请的APP SECRET 如 $secretKey="94dc99566550d87f8fa8ece112xxxxx"
    char secret_key[] = "填写网页上申请的Secret Key";
    // 需要识别的文件
    char *filename = NULL;
    filename = file;

    // 文件后缀仅支持 pcm/wav/amr 格式，极速版额外支持m4a 格式
    char format[] = "pcm";

    char *url = "http://vop.baidu.com/server_api";  // 可改为https

    //  1537 表示识别普通话，使用输入法模型。其它语种参见文档
    int dev_pid = 1537;

    char *scope = "audio_voice_assistant_get"; // # 有此scope表示有asr能力，没有请在网页里勾选，非常旧的应用可能没有

    //测试自训练平台需要打开以下信息， 自训练平台模型上线后，您会看见 第二步：“”获取专属模型参数pid:8001，modelid:1234”，按照这个信息获取 dev_pid=8001，lm_id=1234
    /* dev_pid = 8001 ;   
       int lm_id = 1234 ;
    */

    /* 极速版 打开注释的话请填写自己申请的appkey appSecret ，并在网页中开通极速版（开通后可能会收费）
    url = "http://vop.baidu.com/pro_api"; // 可改为https
    dev_pid = 80001;
    scope = "brain_enhanced_asr"; // 有此scope表示有极速版能力，没有请在网页里开通极速版
    */

    /* 忽略scope检查，非常旧的应用可能没有
    scope = "";
    */

    config->file= fopen(filename, "r");
    if (config->file == NULL) {
        //文件不存在
        char cwd[200];
        getcwd(cwd,sizeof(cwd));
        snprintf(g_demo_error_msg, BUFFER_ERROR_SIZE,
                 "current running directory does not contain file %s, %s", filename, cwd);
        return ERROR_ASR_FILE_NOT_EXIST;
    }

    // 将上述参数填入config中
    snprintf(config->api_key, sizeof(config->api_key), "%s", api_key);
    snprintf(config->secret_key, sizeof(config->secret_key), "%s", secret_key);
    snprintf(config->format, sizeof(config->format), "%s", format);
    snprintf(config->scope, sizeof(config->scope), "%s", scope);
    snprintf(config->url, sizeof(config->url), "%s", url);
    config->rate = 16000; // 采样率固定值
    config->dev_pid = dev_pid;
    snprintf(config->cuid, sizeof(config->cuid), "1234567C");

    return RETURN_OK;
}


// 调用识别接口
RETURN_CODE run_asr(struct asr_config *config, const char *token,char result_voice[]) {
    char url[300];
    CURL *curl = curl_easy_init(); // 需要释放
    char *cuid = curl_easy_escape(curl, config->cuid, strlen(config->cuid)); // 需要释放

    snprintf(url, sizeof(url), "%s?cuid=%s&token=%s&dev_pid=%d",
             config->url, cuid, token, config->dev_pid);

    //测试自训练平台需要打开以下信息
    /*snprintf(url, sizeof(url), "%s?cuid=%s&token=%s&dev_pid=%d&lm_id=%d",
             config->url, cuid, token, config->dev_pid, config->lm_id);*/
    curl_free(cuid);


    struct curl_slist *headerlist = NULL;
    char header[50];
    snprintf(header, sizeof(header), "Content-Type: audio/%s; rate=%d", config->format,
             config->rate);
    headerlist = curl_slist_append(headerlist, header); // 需要释放

    int content_len = 0;
    char *result = NULL;
    char *audio_data = read_file_data(config->file, &content_len); // 读取文件， 需要释放
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);//禁用SSL证书验证
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10); // 连接5s超时
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60); // 整体请求60s超时
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); // 添加http header Content-Type
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, audio_data); // 音频数据
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content_len); // 音频数据长度
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);  // 需要释放

    CURLcode res_curl = curl_easy_perform(curl);


    // printf("request url :%s\n", url);
    // printf("header is: %s\n", header);

    RETURN_CODE res = RETURN_OK;
    if (res_curl != CURLE_OK) {
        // curl 失败
        snprintf(g_demo_error_msg, BUFFER_ERROR_SIZE, "perform curl error:%d, %s.\n", res,
                 curl_easy_strerror(res_curl));
        res = ERROR_ASR_CURL;
    } else {
        //printf("YOUR FINAL RESULT: %s\n", result);
        /*提取识别结果*/
        char result_buf[1024];
        char need_buf[1024];
        strcpy(result_buf,result);//strtok拆分字符串是直接在原串上操作，所以要求参1必须，可读可写（char *str = “www.baidu.com” 不行！！！） 
        strcpy(need_buf,strtok(result_buf,"["));//分离[前的字符串
        strcpy(need_buf,strtok(NULL,"]"));//分离]前的字符串
        printf("识别结果为：%s\n",need_buf);//识别结果
        strcpy(result_voice,need_buf);
    }

    curl_slist_free_all(headerlist);
    free(audio_data);
    free(result);
    curl_easy_cleanup(curl);
    return res;
}

// 从fp读取文件内容，长度保存在content_len
char *read_file_data(FILE *fp, int *content_len) {
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    *content_len = len;
    char *data = (char *) malloc(len);
    if (data == NULL) {
        fprintf(stderr, "malloc size %d failed", len);
        exit(11);
    }
    fread(data, 1, len, fp);
    return data;

}