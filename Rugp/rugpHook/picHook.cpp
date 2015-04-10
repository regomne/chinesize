#include <windows.h>
#include <png.h>
#include <string>

#include "Header.h"
#include "picHook.h"

using namespace std;
DWORD crcTable[256];
wstring r6path=L"r6ti\\";
wstring r7path=L"r7\\";
wstring rip008path=L"r08\\";


ExPic6Routine oldextR6ti;
ExPic7Routine oldExtPicType7;
ExPicR8Routine oldExtPicTypeRip008;

DWORD crc32(DWORD crc,BYTE *buffer, DWORD size)  
{  
    unsigned int i;  
    for (i = 0; i < size; i++) {  
        crc = crcTable[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);  
    }  
    return crc ;  
}

void CopyDib32(BYTE* src,BYTE* dest,int width,int height,int srcStride,int destStride)
{

    for(int i=0;i<height;i++)
    {
        auto ps=(DWORD*)src;
        auto pd=(DWORD*)dest;
        for(int j=0;j<width;j++)
        {
            *pd++=*ps++;
        }
        src-=srcStride;
        dest-=destStride;
    }
}

void FillDib32(BYTE* dest,int width,int height,int stride,DWORD color)
{
    for(int i=0;i<height;i++)
    {
        auto pd=(DWORD*)dest;
        for(int j=0;j<width;j++)
            *pd++=color;
        dest-=stride;
    }
}

void InitCrcTable()
{
    unsigned int c;  
    unsigned int i, j;  

    for (i = 0; i < 256; i++) {  
        c = (unsigned int)i;  
        for (j = 0; j < 8; j++) {  
            if (c & 1)  
                c = 0xedb88320L ^ (c >> 1);  
            else  
                c = c >> 1;  
        }  
        crcTable[i] = c;  
    }  
}

void AlphaBlend1(BYTE* dest,int destStride,BYTE* src,int width,int height,int srcStride)
{
    auto ps=src;
    auto pd=dest;
    for(int i=0;i<height;i++)
    {
        auto pls=ps;
        auto pld=pd;
        for(int j=0;j<width;j++)
        {
            double alSrc=(double)pls[3]/255;
            double alDest=(double)pld[3]/255;
            pld[0]=pls[0]*alSrc+pld[0]*alDest*(1-alSrc);
            pld[1]=pls[1]*alSrc+pld[1]*alDest*(1-alSrc);
            pld[2]=pls[2]*alSrc+pld[2]*alDest*(1-alSrc);
            pld[3]=(alSrc+alDest*(1-alSrc))*255;
            pls+=4;
            pld+=4;
        }
        ps-=srcStride;
        pd-=destStride;
    }
}

void PngRead(png_struct* png,png_byte* buff,png_size_t len)
{
    auto stream=(MemStream*)png_get_io_ptr(png);
    if(stream->cur-stream->start+len > stream->len)
    {
        png_error(png,"err");
    }

    memcpy(buff,stream->cur,len);
    stream->cur+=len;

}

int ReadPngToBmp(MemStream& src,int* width,int* height,int* bit,BYTE** dib)
{
    png_struct* png_ptr;
    png_info* info_ptr;
    int color_type,temp;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);

    if (png_ptr == NULL)
    {
        return -1;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        /* If we get here, we had a problem reading the file */
        return -1;
    }

    png_set_read_fn(png_ptr,&src,PngRead);

    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)width, (png_uint_32*)height,bit, &color_type,
        &temp, NULL, NULL);

    if(*bit!=8 || color_type!=PNG_COLOR_TYPE_RGBA)
        return -1;

    png_set_bgr(png_ptr);

    *dib=new BYTE[*width**height*4];
    if(png_get_rowbytes(png_ptr,info_ptr)!=*width*4)
        __asm int 3

    auto rowPointers=new BYTE*[*height];
    auto p=*dib+(*height-1)*(*width*4);
    for(int i=0;i<*height;i++)
    {
        rowPointers[i]=p;
        p-=*width*4;
    }

    png_read_image(png_ptr,rowPointers);

    png_read_end(png_ptr,info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    delete[] rowPointers;

    return 0;
}



int __cdecl NewExtR6ti(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom)
{
    int right=rightbottom&0xffff;
    int bottom=rightbottom>>16;
    int left=lefttop&0xffff;
    int top=lefttop>>16;
    int width=right-left;
    int height=bottom-top;
    
    DWORD crc=crc32(-1,src,*(DWORD*)(info+0x14));
    wchar_t fname[60];
    wsprintf(fname,L"%x_%x.png",crc,(height<<16)|width);
    DWORD size;
    auto buff=ReadFileToMem((r6path+fname).c_str(),&size);

    if(!buff)
        return oldextR6ti(src,info,newDib,stride,lefttop,rightbottom);

    MemStream stm={buff,buff,size};

    int w,h,bit;
    BYTE* dib;
    int rslt=ReadPngToBmp(stm,&w,&h,&bit,&dib);
    HeapFree(hGlobalHeap,0,buff);
    if(rslt)
    {
        return oldextR6ti(src,info,newDib,stride,lefttop,rightbottom);
    }
    
    AlphaBlend1(newDib-top*stride+left*4,stride,dib+(h-1)*(w*4),w,h,w*4);

    delete[] dib;

    return 0;
}

int __cdecl NewExtR7(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,
                     DWORD lefttop2,DWORD rightbottom2,BOOL unk)
{
    int right=rightbottom&0xffff;
    int bottom=rightbottom>>16;
    int left=lefttop&0xffff;
    int top=lefttop>>16;
    int width=right-left;
    int height=bottom-top;

    if(lefttop!=lefttop2 || rightbottom2!=rightbottom)
        return oldExtPicType7(src,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);

    DWORD crc=crc32(-1,src,*(DWORD*)(info+0x14));
    wchar_t fname[60];
    wsprintf(fname,L"%x_%x.png",crc,(height<<16)|width);
    DWORD size;
    auto buff=ReadFileToMem((r7path+fname).c_str(),&size);

    if(!buff)
        return oldExtPicType7(src,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);

    MemStream stm={buff,buff,size};

    int w,h,bit;
    BYTE* dib;
    int rslt=ReadPngToBmp(stm,&w,&h,&bit,&dib);
    HeapFree(hGlobalHeap,0,buff);
    if(rslt)
    {
        return oldExtPicType7(src,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    }

    AlphaBlend1(newDib-top*stride+left*4,stride,dib+(h-1)*(w*4),w,h,w*4);

    delete[] dib;

    return 0;
}
int __cdecl NewExtRip008(BYTE* src,int srcLen,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,
                     DWORD lefttop2,DWORD rightbottom2,BOOL unk)
{
    int right=rightbottom&0xffff;
    int bottom=rightbottom>>16;
    int left=lefttop&0xffff;
    int top=lefttop>>16;
    int width=right-left;
    int height=bottom-top;

    if(lefttop!=lefttop2 || rightbottom2!=rightbottom)
        return oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);

    auto oldDib=new BYTE[width*height*4];
    if(!oldDib)
        return oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    auto destStart=newDib-top*stride+left*4;
    CopyDib32(destStart,oldDib+(height-1)*(width*4),width,height,stride,width*4);

    FillDib32(destStart,width,height,stride,0);
    oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    auto newDib1=new BYTE[width*height*4];
    CopyDib32(destStart,newDib1+(height-1)*(width*4),width,height,stride,width*4);

    DWORD crc=crc32(-1,newDib1,width*height*4);
    delete[] newDib1;
    CopyDib32(oldDib+(height-1)*(width*4),destStart,width,height,width*4,stride);
    delete[] oldDib;
    wchar_t fname[60];
    wsprintf(fname,L"%x_%x.png",crc,(height<<16)|width);
    DWORD size;
    auto buff=ReadFileToMem((rip008path+fname).c_str(),&size);

    if(!buff)
    {
        return oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    }

    MemStream stm={buff,buff,size};

    int w,h,bit;
    BYTE* dib;
    int rslt=ReadPngToBmp(stm,&w,&h,&bit,&dib);
    HeapFree(hGlobalHeap,0,buff);
    if(rslt)
    {
        return oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    }

    AlphaBlend1(newDib-top*stride+left*4,stride,dib+(h-1)*(w*4),w,h,w*4);

    delete[] dib;

    return 0;
}
