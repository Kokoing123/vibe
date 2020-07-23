#ifndef __BUMINGWU_DETECT_HEADER_H__
#define __BUMINGWU_DETECT_HEADER_H__

#define BUMINGWU_HAS_DLL
#if defined (BUMINGWU_HAS_DLL)
#    if defined (BUMINGWU_BUILD_DLL)
#      define BUMINGWU_API __declspec (dllexport)
#    else
#      define BUMINGWU_API  __declspec (dllimport)
#	endif
#else
#  define BUMINGWU_API
#endif

#include "ViaDef.h"

extern "C"
{
	/*-----------------------��ʼ��--------------------------
	���������
	ParamSt *paramPtr����ʼ������Ҫ��һЩ�����������㷨���͡�gpu��ģ��·����
	��������
	void *handle������һ����ָ���������ݣ����ڱ����㷨����Ҫ��һЩ�ڴ�
	-------------------------------------------------------------*/
	BUMINGWU_API void *InitialBuMingWuFunct(AlgInitSt *paramPtr);


	/*------------------------------�ڴ��ͷ�------------------------------
	���������
	void *handle��ָ���㷨��������ڴ�
	��������
	����Ϊ��
	-----------------------------------------------------------------------------*/
	BUMINGWU_API void ReleaseBuMingWuFunct(void *handle);


	/*------------------------------���/ʶ��------------------------------
	���������
	InDataSt *inDataPtr��ʵʱ�������ݣ�����ͼƬ��Ϣ�����������һЩ��ֵ��Ϣ
	void *handle��ָ���㷨��������ڴ�
	OutDataSt *outDataPtr��ͼ�񾭹��㷨ʶ������õ��Ľ��
	���������
	int flag�������㷨���е�״̬���Ƿ��쳣
	---------------------------------------------------------------------------*/
	BUMINGWU_API int DoBuMingWuObjectFunct(InDataSt *inDataPtr, void *handle, ObjectBoxSurvSt *objBoxeList);


	/*-----------------------------ɾ������ID����Ӧ����Դ---------------------------
	���������
	InDataSt *inDataPtr������ʵʱ��Ϣ����Ҫ�Ǵ�������id
	void *handle��ָ���㷨��������ڴ�
	��������
	��
	-------------------------------------------------------------------------------------------*/
	BUMINGWU_API void DeleteBuMingWuTaskFunct(InDataSt *inDataPtr, void *handle);
}
#endif