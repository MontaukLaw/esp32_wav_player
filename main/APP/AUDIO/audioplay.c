#include "user_comm.h"

TaskHandle_t EKYTask_Handler; /* 任务句柄 */
const char *audio_play_tag = "audio";
void key_task(void *pvParameters); /* 任务函数 */

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

__audiodev g_audiodev; /* 音乐播放控制器 */

extern bool g_playing; /* 是否正在播放 */

/**
 * @brief       开始音频播放
 * @param       无
 * @retval      无
 */
void audio_start(void)
{
    g_audiodev.status = 3 << 0;
    i2s_trx_start();
}

/**
 * @brief       停止音频播放
 * @param       无
 * @retval      无
 */
void audio_stop(void)
{
    g_audiodev.status = 0;
    i2s_trx_stop();
}

/**
 * @brief       得到path路径下，目标文件的总数
 * @param       path : 文件路径
 * @retval      有效文件总数
 */
uint16_t audio_get_tnum(uint8_t *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;
    FILINFO *tfileinfo;

    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO));

    res = f_opendir(&tdir, (const TCHAR *)path);

    if ((res == FR_OK) && tfileinfo)
    {
        while (1)
        {
            res = f_readdir(&tdir, tfileinfo);

            if ((res != FR_OK) || (tfileinfo->fname[0] == 0))
            {
                break;
            }

            res = exfuns_file_type(tfileinfo->fname);

            if ((res & 0xF0) == 0x40)
            {
                rval++; /* 有效文件数增加1 */
            }
        }
    }

    free(tfileinfo);

    return rval;
}

/**
 * @brief       显示曲目索引
 * @param       index : 当前索引
 * @param       total : 总文件数
 * @retval      无
 */
void audio_index_show(uint16_t index, uint16_t total)
{
    spilcd_show_num(30 + 0, 230, index, 3, 16, RED);
    spilcd_show_char(30 + 24, 230, '/', 16, 0, RED);
    spilcd_show_num(30 + 32, 230, total, 3, 16, RED);
}

/**
 * @brief       显示播放时间,比特率 信息
 * @param       totsec : 音频文件总时间长度
 * @param       cursec : 当前播放时间
 * @param       bitrate: 比特率(位速)
 * @retval      无
 */
void audio_msg_show(uint32_t totsec, uint32_t cursec, uint32_t bitrate)
{
    static uint16_t playtime = 0xFFFF;

    if (playtime != cursec)
    {
        playtime = cursec;

        spilcd_show_xnum(30, 210, playtime / 60, 2, 16, 0X80, RED);
        spilcd_show_char(30 + 16, 210, ':', 16, 0, RED);
        spilcd_show_xnum(30 + 24, 210, playtime % 60, 2, 16, 0X80, RED);
        spilcd_show_char(30 + 40, 210, '/', 16, 0, RED);

        spilcd_show_xnum(30 + 48, 210, totsec / 60, 2, 16, 0X80, RED);
        spilcd_show_char(30 + 64, 210, ':', 16, 0, RED);
        spilcd_show_xnum(30 + 72, 210, totsec % 60, 2, 16, 0X80, RED);

        spilcd_show_num(30 + 110, 210, bitrate / 1000, 4, 16, RED);
        spilcd_show_string(30 + 110 + 32, 210, 200, 16, 16, "Kbps", RED);
    }
}

/**
 * @brief       转换
 * @param       fs:文件系统对象
 * @param       clst:转换
 * @retval      =0:扇区号，0:失败
 */
static LBA_t atk_clst2sect(FATFS *fs, DWORD clst)
{
    clst -= 2; /* Cluster number is origin from 2 */

    if (clst >= fs->n_fatent - 2)
    {
        return 0; /* Is it invalid cluster number? */
    }

    return fs->database + (LBA_t)fs->csize * clst; /* Start sector number of the cluster */
}

/**
 * @brief       偏移
 * @param       dp:指向目录对象
 * @param       Offset:目录表的偏移量
 * @retval      FR_OK(0):成功，!=0:错误
 */
FRESULT atk_dir_sdi(FF_DIR *dp, DWORD ofs)
{
    DWORD clst;
    FATFS *fs = dp->obj.fs;

    if (ofs >= (DWORD)((FF_FS_EXFAT && fs->fs_type == FS_EXFAT) ? 0x10000000 : 0x200000) || ofs % 32)
    {
        /* Check range of offset and alignment */
        return FR_INT_ERR;
    }

    dp->dptr = ofs;        /* Set current offset */
    clst = dp->obj.sclust; /* Table start cluster (0:root) */

    if (clst == 0 && fs->fs_type >= FS_FAT32)
    { /* Replace cluster# 0 with root cluster# */
        clst = (DWORD)fs->dirbase;

        if (FF_FS_EXFAT)
        {
            dp->obj.stat = 0;
        }
        /* exFAT: Root dir has an FAT chain */
    }

    if (clst == 0)
    { /* Static table (root-directory on the FAT volume) */
        if (ofs / 32 >= fs->n_rootdir)
        {
            return FR_INT_ERR; /* Is index out of range? */
        }

        dp->sect = fs->dirbase;
    }
    else
    { /* Dynamic table (sub-directory or root-directory on the FAT32/exFAT volume) */
        dp->sect = atk_clst2sect(fs, clst);
    }

    dp->clust = clst; /* Current cluster# */

    if (dp->sect == 0)
    {
        return FR_INT_ERR;
    }

    dp->sect += ofs / fs->ssize;           /* Sector# of the directory entry */
    dp->dir = fs->win + (ofs % fs->ssize); /* Pointer to the entry in the win[] */

    return FR_OK;
}

void key_task(void *pvParameters)
{
    static uint8_t speaker_vol = 0x20; /* 初始音量 */
    uint8_t key;
    pvParameters = pvParameters;
    while (1)
    {
        // delay 1s
        // vTaskDelay(pdMS_TO_TICKS(1000));
        // ESP_LOGI(audio_play_tag, "Key task running");

        key = xl9555_key_scan(0);
        if (key == KEY1_PRES)
        {
            ESP_LOGI(audio_play_tag, "Key pressed");
            // play wave_0_file_name
            if (g_playing)
            {
                ESP_LOGI(audio_play_tag, "Playing no intterupt");
            }
            else
            {
                // wav_play_song(WAVE_3_FILE_NAME);
            }
        }
        else if (key == KEY2_PRES)
        {
            if (speaker_vol == 0x01)
            {
                speaker_vol = 0x00; /* 最小音量 */
            }
            else
            {
                speaker_vol--;
            }
            es8388_spkvol_set(speaker_vol);
            ESP_LOGI(audio_play_tag, "Vol: %d", speaker_vol );
        }
        else if (key == KEY3_PRES)
        {
            speaker_vol++;
            if (speaker_vol > 33)
            {
                speaker_vol = 33; /* 最大音量 */
            }
            es8388_spkvol_set(speaker_vol);

            ESP_LOGI(audio_play_tag, "Vol: %d", speaker_vol );

        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}

/**
 * @brief       播放音乐
 * @param       无
 * @retval      无
 */
void audio_play(void)
{
    uint8_t res;
    FF_DIR wavdir;
    FILINFO *wavfileinfo;
    uint8_t *pname;
    uint16_t totwavnum;
    uint16_t curindex;
    uint8_t key;
    uint32_t temp;
    uint32_t *wavoffsettbl;

    while (f_opendir(&wavdir, "0:/MUSIC"))
    {
        text_show_string(30, 190, 240, 16, "MUSIC文件夹错误!", 16, 0, BLUE);
        vTaskDelay(200);
        spilcd_fill(30, 190, 240, 206, WHITE);
        vTaskDelay(200);
    }

    totwavnum = audio_get_tnum((uint8_t *)"0:/MUSIC"); /* 得到总有效文件数 */

    while (totwavnum == 0)
    {
        text_show_string(30, 190, 240, 16, "没有音乐文件!", 16, 0, BLUE);
        vTaskDelay(200);
        spilcd_fill(30, 190, 240, 146, WHITE);
        vTaskDelay(200);
    }

    wavfileinfo = (FILINFO *)malloc(sizeof(FILINFO));
    pname = malloc(255 * 2 + 1);
    wavoffsettbl = malloc(4 * totwavnum);

    while (!wavfileinfo || !pname || !wavoffsettbl)
    {
        ESP_LOGW(audio_play_tag, "Memory allocation failed");
        // text_show_string(30, 190, 240, 16, "内存分配失败!", 16, 0, BLUE);
        vTaskDelay(200);
        // spilcd_fill(30, 190, 240, 146, WHITE);
        // vTaskDelay(200);
    }

    res = f_opendir(&wavdir, "0:/MUSIC");

    if (res == FR_OK)
    {
        curindex = 0; /* 当前索引为0 */

        while (1)
        {
            temp = wavdir.dptr; /* 记录当前index */

            res = f_readdir(&wavdir, wavfileinfo); /* 读取目录下的一个文件 */

            if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
            {
                break; /* 错误了/到末尾了,退出 */
            }

            res = exfuns_file_type(wavfileinfo->fname);

            if ((res & 0xF0) == 0x40)
            {
                wavoffsettbl[curindex] = temp; /* 记录索引 */
                curindex++;
            }
        }
    }

    // taskENTER_CRITICAL(&my_spinlock);
    // xTaskCreate(key_task, "key", 4096, &EKYTask_Handler, 5, NULL);
    // taskEXIT_CRITICAL(&my_spinlock);

    ESP_LOGI(audio_play_tag, "Enter main while");

    curindex = 0; /* 从0开始显示 */
    res = f_opendir(&wavdir, (const TCHAR *)"0:/MUSIC");

    while (1)
    {

        // ESP_LOGI(audio_play_tag, "Main while loop");
        // delay 1s
        // vTaskDelay(pdMS_TO_TICKS(1000));
        uart_recv();

        vTaskDelay(pdMS_TO_TICKS(10)); /* 延时10ms */
    }

    while (res == FR_OK) /* 打开目录 */
    {
        ESP_LOGI(audio_play_tag, "1");
        atk_dir_sdi(&wavdir, wavoffsettbl[curindex]); /* 改变当前目录索引 */
        ESP_LOGI(audio_play_tag, "2");

        res = f_readdir(&wavdir, wavfileinfo); /* 读取文件 */

        if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
        {
            ESP_LOGE(audio_play_tag, "f_readdir error");
            break;
        }

        ESP_LOGI(audio_play_tag, "3");
        strcpy((char *)pname, "0:/MUSIC/");
        strcat((char *)pname, (const char *)wavfileinfo->fname);
        ESP_LOGI(audio_play_tag, "4");
        spilcd_fill(30, 190, spilcddev.width, spilcddev.height, WHITE);
        audio_index_show(curindex + 1, totwavnum);
        ESP_LOGI(audio_play_tag, "5");
        text_show_string(30, 190, 300, 16, (char *)wavfileinfo->fname, 16, 0, BLUE);
        ESP_LOGI(audio_play_tag, "6");
        // key = audio_play_song(pname);

        ESP_LOGI(audio_play_tag, "Playing: %s", pname);

        // delay 1s
        vTaskDelay(pdMS_TO_TICKS(1000));

        // 打印系统剩余内存
        // ESP_LOGI(audio_play_tag, "Free heap size: %d ", esp_get_free_heap_size());
        // ESP_LOGI(audio_play_tag, "Internal free heap size: %d ", esp_get_free_heap_size());

        // if (key == KEY1_PRES) /* 上一首 */
        // {
        //     ESP_LOGI(audio_play_tag, "KEY1");
        //     if (curindex)
        //     {
        //         curindex--;
        //     }
        //     else
        //     {
        //         curindex = totwavnum - 1;
        //     }
        // }
        // else if (key == KEY0_PRES) /* 下一首 */
        // {
        //     ESP_LOGI(audio_play_tag, "KEY0");
        //     curindex++;

        //     if (curindex >= totwavnum)
        //     {
        //         curindex = 0;
        //     }
        // }
        // else
        // {
        //     break;
        // }
    }

    free(wavfileinfo);
    free(pname);
    free(wavoffsettbl);
}

/**
 * @brief       播放某个音频文件
 * @param       fname : 文件名
 * @retval      按键值
 *   @arg       KEY0_PRES , 下一曲.
 *   @arg       KEY1_PRES , 上一曲.
 *   @arg       其他 , 错误
 */
uint8_t audio_play_song(uint8_t *fname)
{
    uint8_t res;

    res = exfuns_file_type((char *)fname);

    switch (res)
    {
    case T_WAV:
        res = wav_play_song(fname);
        break;
    case T_MP3:
        /* 自行实现 */
        break;

    default: /* 其他文件,自动跳转到下一曲 */
        printf("can't play:%s\r\n", fname);
        res = KEY0_PRES;
        break;
    }
    return res;
}
