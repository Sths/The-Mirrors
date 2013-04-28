#ifndef CHC_TEXTURE_H
#define CHC_TEXTURE_H

#include <Windows.h>
#include <glut.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <glaux.h>
#include <string>
#include <olectl.h>                                                       // OLE���ƿ�ͷ�ļ� 

//����һ��.bmp��ʽ����ͼ���� 
static int LoadGLTextures(GLuint& unTexture, const char* chFileName)                
{
    AUX_RGBImageRec *TextureImage;                    //������ͼ���ݵ�ָ��
    TextureImage = auxDIBImageLoad(chFileName); //������ͼ����

    glGenTextures(1, &unTexture);                    // ����һ������unTexture
    glBindTexture(GL_TEXTURE_2D, unTexture);        //������Ȼ��Ը������������������

    //�����������Ϣ��
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage->sizeX, TextureImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);//����λͼ��С������ʽ��
    glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);		//�����˲�Ϊ�����˲�
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);    //�����˲�

    if (TextureImage) {      //�ͷ���Դ
        if (TextureImage->data) {
            free(TextureImage->data);
        }
        free(TextureImage);
    }
    return TRUE;
}

static BOOL BuildTexture_new(GLuint &texid, char *szPathName)                      // ����ͼƬ��ת��Ϊ����   
{   
    HDC         hdcTemp;                                                // DC��������λͼ   
    HBITMAP     hbmpTemp;                                               // ������ʱλͼ   
    IPicture    *pPicture;                                              // ����IPicture Interface   
    OLECHAR     wszPath[MAX_PATH+1];                                    // ͼƬ����ȫ·��   
    char        szPath[MAX_PATH+1];                                     // ͼƬ����ȫ·��   
    long        lWidth;                                                 // ͼ����   
    long        lHeight;                                                // ͼ��߶�   
    long        lWidthPixels;                                           // ͼ��Ŀ��(������Ϊ��λ)   
    long        lHeightPixels;                                          // ͼ��ĸߴ�(������Ϊ��λ)   
    GLint       glMaxTexDim ;                                           // ������������ߴ�   
   
    if (strstr(szPathName, "http://"))                                  // ���·������ http:// ��...   
    {   
        strcpy(szPath, szPathName);                                     // ��·�������� szPath   
    }   
    else                                                                // ������ļ�����ͼƬ   
    {   
        GetCurrentDirectory(MAX_PATH, szPath);                          // ȡ�õ�ǰ·��   
        strcat(szPath, "\\");                                           // ����ַ�"\"   
        strcat(szPath, szPathName);                                     // ���ͼƬ�����·��   
    }   
   
    MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);      // ��ASCII��ת��ΪUnicode��׼��   
    HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);   
   
    if(FAILED(hr))                                                      // �������ʧ��   
    {   
        // ͼƬ����ʧ�ܳ�����Ϣ   
        MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);   
        return FALSE;                                                   // ���� FALSE   
    }   
   
    hdcTemp = CreateCompatibleDC(GetDC(0));                             // ���������豸������   
    if(!hdcTemp)                                                        // ����ʧ��?   
    {   
        pPicture->Release();                                         // �ͷ�IPicture   
        // ͼƬ����ʧ�ܳ�����Ϣ   
        MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);   
        return FALSE;                                                   // ���� FALSE   
    }   
   
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);                   // ȡ��֧�ֵ��������ߴ�   
       
    pPicture->get_Width(&lWidth);                                        // ȡ��IPicture ��� (ת��ΪPixels��ʽ)   
    lWidthPixels    = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);   
    pPicture->get_Height(&lHeight);                                      // ȡ��IPicture �߶� (ת��ΪPixels��ʽ)   
    lHeightPixels   = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);   
   
    // ����ͼƬ����õ�Ч��   
    if (lWidthPixels <= glMaxTexDim)                                 // ͼƬ����Ƿ񳬹��Կ����֧�ֳߴ�   
        lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f);    
    else                                                                // ����,��ͼƬ�����Ϊ�Կ����֧�ֳߴ�   
        lWidthPixels = glMaxTexDim;   
    
    if (lHeightPixels <= glMaxTexDim)                                    // ͼƬ�߶��Ƿ񳬹��Կ����֧�ֳߴ�   
        lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);   
    else                                                                // ����,��ͼƬ�߶���Ϊ�Կ����֧�ֳߴ�   
        lHeightPixels = glMaxTexDim;   
   
    // ����һ����ʱλͼ   
    BITMAPINFO  bi = {0};                                               // λͼ������   
    DWORD       *pBits = 0;                                             // ָ��λͼBits��ָ��   
   
    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);             // ���ýṹ��С   
    bi.bmiHeader.biBitCount     = 32;                                   // 32 λ   
    bi.bmiHeader.biWidth        = lWidthPixels;                         // �������ֵ   
    bi.bmiHeader.biHeight       = lHeightPixels;                        // �߶�����ֵ   
    bi.bmiHeader.biCompression  = BI_RGB;                               // RGB ��ʽ   
    bi.bmiHeader.biPlanes       = 1;                                    // һ��λƽ��   
   
    // ����һ��λͼ�������ǿ���ָ����ɫ����� ������ÿλ��ֵ   
    hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);   
       
    if(!hbmpTemp)                                                       // ����ʧ��?   
    {   
        DeleteDC(hdcTemp);                                              // ɾ���豸������   
        pPicture->Release();											// �ͷ�IPicture   
        // ͼƬ����ʧ�ܳ�����Ϣ   
        MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);   
        return FALSE;                                                   // ���� FALSE   
    }   
   
    SelectObject(hdcTemp, hbmpTemp);                                    // ѡ����ʱDC�������ʱλͼ����   
   
    // ��λͼ�ϻ���IPicture   
    pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);   
   
    // ��BGRת��ΪRGB����ALPHAֵ��Ϊ255   
    for(long i = 0; i < lWidthPixels * lHeightPixels; i++)               // ѭ���������е�����   
    {   
        BYTE* pPixel    = (BYTE*)(&pBits[i]);                           // ��ȡ��ǰ����   
        BYTE  temp      = pPixel[0];                                    // ��ʱ�洢��һ����ɫ����(��ɫ)   
        pPixel[0]       = pPixel[2];                                    // ����ɫֵ�浽��һλ   
        pPixel[2]       = temp;                                         // ����ɫֵ�浽����λ   
        pPixel[3]       = 255;                                          // ALPHAֵ��Ϊ255   
    }   
   
    glGenTextures(1, &texid);                                           // ��������   
   
    // ʹ������λͼ�������� �ĵ�������   
    glBindTexture(GL_TEXTURE_2D, texid);                                // ������   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // �����˲�   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // �����˲�   
   
    // ��������   
    glTexImage2D(GL_TEXTURE_2D, 0, 3, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);   
   
    DeleteObject(hbmpTemp);                                             // ɾ������   
    DeleteDC(hdcTemp);                                                  // ɾ���豸������   
   
    pPicture->Release();												// �ͷ� IPicture   
   
    return TRUE;                                                        // ���� TRUE   
}   

#endif CHC_TEXTURE_H