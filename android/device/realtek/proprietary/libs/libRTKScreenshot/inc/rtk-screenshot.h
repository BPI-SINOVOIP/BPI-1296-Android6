#ifndef __RTK_RECORD_H__
#define __RTK_RECORD_H__

class RTKScreenshotPrivte;

class RTKScreenshot {
public:
    RTKScreenshot(int32_t displayId, int crop_width, int crop_height);
    virtual ~RTKScreenshot();

    int     update(void);

    /* output format : ARGB8888 */
    const void *  getVirAddr      (void);
    int     getWidth        (void);
    int     getHeight       (void);
    int     getStride       (void);
    int     getFormat       (void);
    unsigned long getSize   (void);

private:
    RTKScreenshotPrivte * mPrivate;
};


#endif /* End of __RTK_RECORD_H__ */
