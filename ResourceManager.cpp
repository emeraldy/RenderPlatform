//-----------------------------------------------------------------
// Resource Manager class
// C++ Source - ResourceManager.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ResourceManager.h"

//-----------------------------------------------------------------
// helper function definitions
//-----------------------------------------------------------------
//callback function for libpng reading from memory
void LoadPNGFromMemory(png_structp png_ptr, png_bytep data, png_size_t length);

//-----------------------------------------------------------------
// Class function definitions
//-----------------------------------------------------------------
ResourceManager::ResourceManager()
{
    //text resource pool initialisation
    m_iTextResourceCount = 2;
    m_lppwTextResources = new WCHAR * [m_iTextResourceCount];
    for (int i = 0; i < m_iTextResourceCount; i++)
        m_lppwTextResources[i] = NULL;
    m_iCurrentTextResourceIndex = 0;

    //GLSL effect pool initialisation
    m_iGLSLEffectResourceCount = 1;
    m_pGLSLEffectResources = new GLSLEffect[m_iGLSLEffectResourceCount];
    for (int i = 0; i < m_iGLSLEffectResourceCount; i++)
    {
        m_pGLSLEffectResources[i].pGLSLShaders = NULL;//always initialise a pointer to null if you are going to new it later!!
        m_pGLSLEffectResources[i].lppwAttribNames = NULL;
    }
    m_iCurrentGLSLEffectResourceIndex = 0;

    //mesh pool initialisation
    m_iMeshResourceCount = 2;
    m_pMeshResources = new Mesh[m_iMeshResourceCount];
    for (int i = 0; i < m_iMeshResourceCount; i++)
    {
        m_pMeshResources[i].pfPositions = NULL;
        m_pMeshResources[i].pfColours = NULL;
    }
    m_iCurrentMeshResourceIndex = 0;

    //texture2D pool initialisation
    m_iTexture2DResourceCount = 1;
    m_pTexture2DResources = new Texture2D[m_iTexture2DResourceCount];
    for (int i = 0; i < m_iTexture2DResourceCount; i++)
    {
        m_pTexture2DResources[i].data = NULL;
    }
    m_iCurrentTexture2DResourceIndex = 0;

    m_pAssetIOHandler = new AssetIOHandler;
}

ResourceManager::~ResourceManager()
{
    //release text pool
    for (int i = 0; i < m_iTextResourceCount; i++)
        SAFE_DELETEARRAY(m_lppwTextResources[i]);
    SAFE_DELETEARRAY(m_lppwTextResources);

    //release GLSL effect pool
    DestroyGLSLEffectPool();

    //relese mesh pool
    for (int i = 0; i < m_iMeshResourceCount; i++)
    {
        SAFE_DELETEARRAY(m_pMeshResources[i].pfPositions);
        SAFE_DELETEARRAY(m_pMeshResources[i].pfColours);
    }
    SAFE_DELETEARRAY(m_pMeshResources);

    //release texture2D pool
    for (int i = 0; i < m_iTexture2DResourceCount; i++)
    {
        SAFE_DELETEARRAY(m_pTexture2DResources[i].data);
    }
    SAFE_DELETEARRAY(m_pTexture2DResources);

    SAFE_DELETE(m_pAssetIOHandler);
}

int ResourceManager::GenerateTextResource(LPCWSTR lpcwFileName, LPCWSTR lpcwType)
{
    if (m_iCurrentTextResourceIndex > m_iTextResourceCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new text. Resource pool limit reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    if (m_pAssetIOHandler->LoadAsset(lpcwFileName) == FALSE)
        return -1;
    //copy text to resource pool from asset io handler's local buffer
    if (lstrcmpi(lpcwType, L"ascii") == 0)//in ANSI ASCII, convert to WCHAR
    {
        m_lppwTextResources[m_iCurrentTextResourceIndex] = new WCHAR[m_pAssetIOHandler->GetBufferSize() + 1];//one byte is one character
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
            (LPCSTR)m_pAssetIOHandler->GetBuffer(), -1,
            m_lppwTextResources[m_iCurrentTextResourceIndex], m_pAssetIOHandler->GetBufferSize());

        //add null terminator for the text string
        m_lppwTextResources[m_iCurrentTextResourceIndex][m_pAssetIOHandler->GetBufferSize()] = L'\0';
    }
    else
    {
        //now assume else case is only that the file is encoded in UTF-16 already (need to test this branch in the future)
        m_lppwTextResources[m_iCurrentTextResourceIndex] = new WCHAR[m_pAssetIOHandler->GetBufferSize() / sizeof(WCHAR) + 1];//every two bytes are one character
        const BYTE* pbRawData = m_pAssetIOHandler->GetBuffer();
        int i, j;
        for (i = 0, j = 0; i < int(m_pAssetIOHandler->GetBufferSize() / sizeof(WCHAR)); i++, j += 2)
            m_lppwTextResources[m_iCurrentTextResourceIndex][i] = pbRawData[j];

        //add null terminator for the text string
        m_lppwTextResources[m_iCurrentTextResourceIndex][m_pAssetIOHandler->GetBufferSize() / sizeof(WCHAR)] = L'\0';
    }
    //clear asset io handler's local buffer
    m_pAssetIOHandler->SetBuffer(NULL);
    m_pAssetIOHandler->SetBufferSize(0);


    m_iCurrentTextResourceIndex++;
    return m_iCurrentTextResourceIndex - 1;//return the index for this text in the pool
}

void ResourceManager::DestroyGLSLEffectPool()
{
    for (int i = 0; i < m_iGLSLEffectResourceCount; i++)
    {
        SAFE_DELETEARRAY(m_pGLSLEffectResources[i].pGLSLShaders);
        SAFE_DELETEARRAY(m_pGLSLEffectResources[i].lppwAttribNames);
    }
    SAFE_DELETEARRAY(m_pGLSLEffectResources);
}

int ResourceManager::GenerateGLSLEffectResource(GLSLShader* pglsRequiredShaders, int iRequiredShaderCount, LPWSTR lpwToName, LPWSTR* lppwAttribs, int iAttribNum)
{
    if (m_iCurrentGLSLEffectResourceIndex > m_iGLSLEffectResourceCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new GLSL effect. Resource pool limit reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].iGLShaderCount = iRequiredShaderCount;
    m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].lpwEffectName = lpwToName;
    m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].pGLSLShaders = new GLSLShader[iRequiredShaderCount];
    //copy each required shader
    for (int i = 0; i < iRequiredShaderCount; i++)
    {
        m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].pGLSLShaders[i].gluiType = pglsRequiredShaders[i].gluiType;
        m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].pGLSLShaders[i].lpwFileName = pglsRequiredShaders[i].lpwFileName;
        m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].pGLSLShaders[i].iIndex = GenerateTextResource(pglsRequiredShaders[i].lpwFileName, L"ascii");
        if (m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].pGLSLShaders[i].iIndex == -1)
            return -1;
    }

    //copy attribute names
    m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].lppwAttribNames = new LPWSTR[iAttribNum];
    for (int i = 0; i < iAttribNum; i++)
    {
        m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].lppwAttribNames[i] = lppwAttribs[i];//string literals are stored statically so can just copy pointers
    }
    m_pGLSLEffectResources[m_iCurrentGLSLEffectResourceIndex].iAttribCount = iAttribNum;

    m_iCurrentGLSLEffectResourceIndex++;
    return m_iCurrentGLSLEffectResourceIndex - 1;//return the inexe for this GLSL effect in the pool
}

int ResourceManager::GenerateMeshResource(Mesh* pMeshData)
{
    if (m_iCurrentMeshResourceIndex > m_iMeshResourceCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new Mesh. Resource pool limit reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    //copy the mesh data into the resource pool
    m_pMeshResources[m_iCurrentMeshResourceIndex].iVertexCount = pMeshData->iVertexCount;
    m_pMeshResources[m_iCurrentMeshResourceIndex].iVertexSize = pMeshData->iVertexSize;
    m_pMeshResources[m_iCurrentMeshResourceIndex].pfPositions = new float[pMeshData->iVertexCount * pMeshData->iVertexSize];
    for (int i = 0; i < pMeshData->iVertexCount * pMeshData->iVertexSize; i++)
        m_pMeshResources[m_iCurrentMeshResourceIndex].pfPositions[i] = pMeshData->pfPositions[i];

    m_pMeshResources[m_iCurrentMeshResourceIndex].pfColours = new float[pMeshData->iVertexCount * pMeshData->iVertexSize];
    for (int i = 0; i < pMeshData->iVertexCount * pMeshData->iVertexSize; i++)
        m_pMeshResources[m_iCurrentMeshResourceIndex].pfColours[i] = pMeshData->pfColours[i];

    m_iCurrentMeshResourceIndex++;
    return m_iCurrentMeshResourceIndex - 1;
}
void LoadPNGFromMemory(png_structp png_ptr, png_bytep data, png_size_t length)
{
    //get the buffer head
    png_voidp pSourceBuffer = png_get_io_ptr(png_ptr);

    //read data
    memcpy(data, pSourceBuffer, length);

    //move the buffer head for future read
    png_bytep pNextStart = (png_bytep)pSourceBuffer + length;
    png_set_read_fn(png_ptr, (png_voidp)pNextStart, LoadPNGFromMemory);
}
int ResourceManager::GenerateTexture2DResource(LPCWSTR lpcwFileName, LPCWSTR lpcwType)
{
    if (m_iCurrentTexture2DResourceIndex > m_iTexture2DResourceCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new Texture. Resource pool limit reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    if (m_pAssetIOHandler->LoadAsset(lpcwFileName) == FALSE)
        return -1;

    //process image data stream based on image type
    if (lstrcmpi(lpcwType, L"jpeg") == 0)
    {
        jpeg_decompress_struct jpegObject;
        jpeg_error_mgr jpegErrorManager;

        //set up the normal JPEG error routines
        jpegObject.err = jpeg_std_error(&jpegErrorManager);
        //jpegErrorManager.error_exit = JpegFatalError;
        //jpegErrorManager.emit_message = JpegErrorMessageEmitter;

        //initialise the JPEG decompression object
        jpeg_create_decompress(&jpegObject);

        //specify raw image data source
        jpeg_mem_src(&jpegObject, (unsigned char*)m_pAssetIOHandler->GetBuffer(), m_pAssetIOHandler->GetBufferSize());

        //parse jpeg header. passed TRUE to reject a tables-only JPEG file 
        jpeg_read_header(&jpegObject, TRUE);

        //start decompressor
        jpeg_start_decompress(&jpegObject);

        //image size info
        int iImageWidth = jpegObject.output_width;
        int iImageHeight = jpegObject.output_height;
        int iChannelNum = jpegObject.output_components;
        int iRow_stride = iImageWidth * iChannelNum;//row width in output buffer in byte (jpeg assumes one channel one byte)

        //allocate temporary space for texture data
        BYTE** rows = new BYTE * [iImageHeight];
        for (int i = 0; i < iImageHeight; i++)
            rows[i] = new BYTE[iRow_stride];

        //read rows
        while (jpegObject.output_scanline < jpegObject.output_height) {
            /* jpeg_read_scanlines expects an array of pointers to scanlines.
             * Here the array is only one element long, but you could ask for
             * more than one scanline at a time if that's more convenient.
             */
            jpeg_read_scanlines(&jpegObject, &(rows[jpegObject.output_scanline]), 1);
        }

        /* Finish decompression */
        jpeg_finish_decompress(&jpegObject);

        /* Release JPEG decompression object */
        /* This is an important step since it will release a good deal of memory. */
        jpeg_destroy_decompress(&jpegObject);

        //transfer image data from temporary storage to texture2d resource pool
        m_pTexture2DResources[m_iCurrentTexture2DResourceIndex].data = new BYTE[iImageWidth * iImageHeight * iChannelNum];
        for (int i = 0; i < iImageHeight; i++)
        {
            memcpy(m_pTexture2DResources[m_iCurrentTexture2DResourceIndex].data + i * iRow_stride, rows[i], iRow_stride);
        }

        /* And we're done! */
        //clear temporary row buffer
        for (int i = 0; i < iImageHeight; i++)
        {
            SAFE_DELETEARRAY(rows[i]);
        }
        SAFE_DELETEARRAY(rows);

        //clear asset io handler's local buffer
        m_pAssetIOHandler->SetBuffer(NULL);
        m_pAssetIOHandler->SetBufferSize(0);

        m_iCurrentTexture2DResourceIndex++;
        return m_iCurrentTexture2DResourceIndex - 1;
    }
    else if (lstrcmpi(lpcwType, L"png") == 0)
    {
        //create png_struct
        png_structp pPngstruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (pPngstruct == NULL)
            return -1;

        //create png_info
        png_infop pPnginfo = png_create_info_struct(pPngstruct);
        if (pPnginfo == NULL)
        {
            png_destroy_read_struct(&pPngstruct, NULL, NULL);
            return -1;
        }

        //set the callback function for customised image source read
        png_set_read_fn(pPngstruct, (png_voidp)m_pAssetIOHandler->GetBuffer(), LoadPNGFromMemory);

        //read png info
        png_read_info(pPngstruct, pPnginfo);
        int iImageWidth = png_get_image_width(pPngstruct, pPnginfo);
        int iImageHeight = png_get_image_height(pPngstruct, pPnginfo);
        int iColourType = png_get_color_type(pPngstruct, pPnginfo);
        int iBitDepth = png_get_bit_depth(pPngstruct, pPnginfo);
        int iRow_stride = png_get_rowbytes(pPngstruct, pPnginfo);//row width in output buffer in byte
        int interlace_type = png_get_interlace_type(pPngstruct, pPnginfo);

        //allocate temporary space for texture data
        png_bytep* rows = new png_bytep[iImageHeight];
        for (int i = 0; i < iImageHeight; i++)
            rows[i] = new png_byte[iRow_stride];

        //read actual image data
        png_read_image(pPngstruct, rows);

        png_read_end(pPngstruct, NULL);

        png_destroy_read_struct(&pPngstruct, &pPnginfo, NULL);

        //transfer image data from temporary storage to texture2d resource pool
        m_pTexture2DResources[m_iCurrentTexture2DResourceIndex].data = new BYTE[iImageHeight * iRow_stride];
        for (int i = 0; i < iImageHeight; i++)
        {
            memcpy(m_pTexture2DResources[m_iCurrentTexture2DResourceIndex].data + i * iRow_stride, rows[i], iRow_stride);
        }

        /* And we're done! */
        //clear temporary row buffer
        for (int i = 0; i < iImageHeight; i++)
        {
            SAFE_DELETEARRAY(rows[i]);
        }
        SAFE_DELETEARRAY(rows);

        //clear asset io handler's local buffer
        m_pAssetIOHandler->SetBuffer(NULL);
        m_pAssetIOHandler->SetBufferSize(0);

        m_iCurrentTexture2DResourceIndex++;
        return m_iCurrentTexture2DResourceIndex - 1;
    }
    else
    {
        //for future extension of more image format supports, but for now
        return -1;
    }
}

