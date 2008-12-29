/* Copyright (c) 2008 Peter Palotas
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */
#include "StdAfx.h"

#include "VsBackup.h"
#include "VssBackupComponents.h"
#include "VssAsync.h"

#include "Utils.h"
#include "Macros.h"
#include "VssSnapshotContext.h"
#include "VssProviderProperties.h"

using namespace System::Collections::Generic;
using namespace System::Security::Permissions;

namespace Alphaleonis { namespace Win32 { namespace Vss
{
	VssBackupComponents::VssBackupComponents()
		: mBackup(0), 
		mWriterMetadata(nullptr),
		mWriterComponents(nullptr),
		mWriterStatus(nullptr)
	{
		mWriterMetadata = gcnew WriterMetadataList(this);
		mWriterComponents = gcnew WriterComponentsList(this);
		mWriterStatus = gcnew WriterStatusList(this);

		pin_ptr<IVssBackupComponents *> pVssObject = &mBackup;
		CheckCom( CreateVssBackupComponents(pVssObject) );
	}

	VssBackupComponents::~VssBackupComponents()
	{
		this->!VssBackupComponents();
	}

	VssBackupComponents::!VssBackupComponents()
	{
		if (mBackup != 0)
		{
			mBackup->Release();
			mBackup = 0;
		}
	}

	bool VssBackupComponents::IsVolumeSnapshotted(String ^ volumeName)
	{
		LONG lSnapshotCapability = 0;
		BOOL bSnapshotsPresent = 0;
		CheckCom(::IsVolumeSnapshotted((VSS_PWSZ)((const wchar_t *)NoNullAutoMStr(volumeName)), &bSnapshotsPresent, &lSnapshotCapability));	
		return bSnapshotsPresent != 0;
	}

	VssSnapshotCompatibility VssBackupComponents::GetSnapshotCompatibility(String^ volumeName)
	{
		LONG lSnapshotCapability = 0;
		BOOL bSnapshotsPresent = 0;
		CheckCom(::IsVolumeSnapshotted((VSS_PWSZ)((const wchar_t *)NoNullAutoMStr(volumeName)), &bSnapshotsPresent, &lSnapshotCapability));
		if (!bSnapshotsPresent)
			throw gcnew InvalidOperationException("No snapshot exists for the specified volume");
		return (VssSnapshotCompatibility)lSnapshotCapability;
	}

	bool VssBackupComponents::ShouldBlockRevert(String ^ volumeName)
	{
		// According to MSDN this method is supported also on Windows 2003, however it is not 
		// present in the header-files or library files, except for the library files for ws03 
		// in the vshadow sample directory in the VSSSDK72. Requiring WS08 here.
#if ALPHAVSS_TARGET == ALPHAVSS_TARGET_WIN2008
		OsInfo::RequireAtLeastInFamily(OsVersion::Win2008);
		bool bBlock = 0;
		CheckCom(::ShouldBlockRevert(NoNullAutoMStr(volumeName), &bBlock));
		return bBlock != 0;
#else
		throw gcnew NotSupportedException(L"This method requires Windows Server 2008.");
#endif
	}

	void VssBackupComponents::AbortBackup()
	{
		CheckCom(mBackup->AbortBackup());
	}

	void VssBackupComponents::AddAlternativeLocationMapping(Guid writerId, VssComponentType componentType, String ^ logicalPath, String ^ componentName, String ^ path, String ^ filespec, bool recursive, String ^ destination)
	{
		CheckCom(mBackup->AddAlternativeLocationMapping(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, 
			AutoMStr(logicalPath), NoNullAutoMStr(componentName), NoNullAutoMStr(path),
			NoNullAutoMStr(filespec), recursive, NoNullAutoMStr(destination)));
	}

	void VssBackupComponents::AddComponent(Guid instanceId, Guid writerId, VssComponentType componentType, String ^ logicalPath, String ^ componentName)
	{
		CheckCom(mBackup->AddComponent(ToVssId(instanceId), ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, 
			AutoMStr(logicalPath), NoNullAutoMStr(componentName)));
	}

	void VssBackupComponents::AddNewTarget(Guid writerId, VssComponentType componentType, String ^ logicalPath, String ^ componentName, String ^ path, String ^ fileName, bool recursive, String ^ alternatePath)
	{
#if ALPHAVSS_TARGET >= ALPHAVSS_TARGET_WIN2003
		OsInfo::RequireAtLeast(OsVersion::Win2003);		
		CheckCom(mBackup->AddNewTarget(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType,
			AutoMStr(logicalPath), NoNullAutoMStr(componentName),
			NoNullAutoMStr(path), NoNullAutoMStr(fileName),
			recursive, NoNullAutoMStr(alternatePath)));
#else
		UnsupportedOs();
#endif
	}

	void VssBackupComponents::AddRestoreSubcomponent(Guid writerId, VssComponentType componentType, String^ logicalPath, String ^componentName, String^ subcomponentLogicalPath, String^ subcomponentName)
	{
		CheckCom(mBackup->AddRestoreSubcomponent(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType,
			AutoMStr(logicalPath),
			NoNullAutoMStr(componentName),
			NoNullAutoMStr(subcomponentLogicalPath),
			NoNullAutoMStr(subcomponentName),
			false));
	}

	Guid VssBackupComponents::AddToSnapshotSet(String ^ volumeName, Guid providerId)
	{
		VSS_ID idSnapshot;
		CheckCom(mBackup->AddToSnapshotSet(NoNullAutoMStr(volumeName), ToVssId(providerId), &idSnapshot));
		return ToGuid(idSnapshot);
	}

	[SecurityPermissionAttribute(SecurityAction::LinkDemand)]
	VssAsync ^ VssBackupComponents::BackupComplete()
	{
		IVssAsync *pAsync;
		CheckCom(mBackup->BackupComplete(&pAsync));
		return VssAsync::Adopt(pAsync);
	}

	void VssBackupComponents::BreakSnapshotSet(Guid snapshotSetId)
	{
		CheckCom(mBackup->BreakSnapshotSet(ToVssId(snapshotSetId)));
	}

	int VssBackupComponents::DeleteSnapshots(Guid sourceObjectId, VssObjectType sourceObjectType, bool forceDelete)
	{
		if (sourceObjectType != VssObjectType::Snapshot && sourceObjectType != VssObjectType::SnapshotSet)
			throw gcnew ArgumentException(L"Invalid object type. Must be Snapshot or SnapshotSet", L"sourceObjectType");

		LONG lDeletedSnapshots;
		VSS_ID nonDeletedSnapshotID;
		HRESULT hr = mBackup->DeleteSnapshots(ToVssId(sourceObjectId), (VSS_OBJECT_TYPE)sourceObjectType, forceDelete, &lDeletedSnapshots, &nonDeletedSnapshotID);
		
		if (FAILED(hr))
		{
			throw gcnew VssDeleteSnapshotsFailedException(lDeletedSnapshots, ToGuid(nonDeletedSnapshotID), GetExceptionForHr(hr));
		}

		return lDeletedSnapshots;
	}

	void VssBackupComponents::DisableWriterClasses(array<Guid> ^ writerClassIds)
	{
		CheckCom(mBackup->DisableWriterClasses(VssIds(writerClassIds), writerClassIds->Length));
	}

	void VssBackupComponents::DisableWriterInstances(array<Guid> ^ writerInstanceIds)
	{
		CheckCom(mBackup->DisableWriterInstances(VssIds(writerInstanceIds), writerInstanceIds->Length));
	}

	VssAsync ^ VssBackupComponents::DoSnapshotSet()
	{
		IVssAsync *vssAsync;
		CheckCom(mBackup->DoSnapshotSet(&vssAsync));
		return VssAsync::Adopt(vssAsync);
	}

	void VssBackupComponents::EnableWriterClasses(array<Guid> ^ writerClassIds)
	{
		CheckCom(mBackup->EnableWriterClasses(VssIds(writerClassIds), writerClassIds->Length));
	}

	String ^ VssBackupComponents::ExposeSnapshot(Guid snapshotId, String ^ pathFromRoot, VssVolumeSnapshotAttributes attributes, String ^ expose)
	{
		AutoPwsz pwszExposed;

		CheckCom(mBackup->ExposeSnapshot(ToVssId(snapshotId), AutoMStr(pathFromRoot), (LONG)attributes,
			AutoMStr(expose), &pwszExposed));

		return pwszExposed;
	}

	void VssBackupComponents::FreeWriterMetadata()
	{
		CheckCom(mBackup->FreeWriterMetadata());
	}

	void VssBackupComponents::FreeWriterStatus()
	{
		CheckCom(mBackup->FreeWriterStatus());
	}

	VssAsync ^ VssBackupComponents::GatherWriterMetadata()
	{
		IVssAsync *vssAsync;
		CheckCom(mBackup->GatherWriterMetadata(&vssAsync));
		return VssAsync::Adopt(vssAsync);
	}

	VssAsync ^ VssBackupComponents::GatherWriterStatus()
	{
		IVssAsync *vssAsync;
		CheckCom(mBackup->GatherWriterStatus(&vssAsync));
		return VssAsync::Adopt(vssAsync);
	}

	VssSnapshotProperties ^ VssBackupComponents::GetSnapshotProperties(Guid snapshotId)
	{
		VSS_SNAPSHOT_PROP prop;
		CheckCom(mBackup->GetSnapshotProperties(ToVssId(snapshotId), &prop));
		return VssSnapshotProperties::Adopt(&prop);
	}

	VssBackupComponents::WriterStatusList::WriterStatusList(VssBackupComponents^ backupComponents)
		: mBackupComponents(backupComponents)
	{
	}

	int VssBackupComponents::WriterStatusList::Count::get()
	{
		if (mBackupComponents->mBackup == 0)
			throw gcnew ObjectDisposedException("Instance of IList used after the object creating it was disposed.");

		UINT cWriters;
		CheckCom(mBackupComponents->mBackup->GetWriterStatusCount(&cWriters));
		return (int)cWriters;
	}

	VssWriterStatus^ VssBackupComponents::WriterStatusList::default::get(int index)
	{
		if (index < 0 || index > Count)
			throw gcnew ArgumentOutOfRangeException("index");

		if (mBackupComponents->mBackup == 0)
			throw gcnew ObjectDisposedException("Instance of IList used after the object creating it was disposed.");

		VSS_ID idInstance, idWriter;
		AutoBStr bstrWriter;
		VSS_WRITER_STATE eState;
		HRESULT hrResultFailure;
		CheckCom(mBackupComponents->mBackup->GetWriterStatus(index, &idInstance, &idWriter, &bstrWriter, &eState, &hrResultFailure));
		return gcnew VssWriterStatus(ToGuid(idInstance), ToGuid(idWriter), bstrWriter, (VssWriterState)eState, (VssWriterFailure)hrResultFailure);
	}


	VssBackupComponents::WriterComponentsList::WriterComponentsList(VssBackupComponents^ backupComponents)
		: mBackupComponents(backupComponents)
	{
	}

	int VssBackupComponents::WriterComponentsList::Count::get()
	{
		if (mBackupComponents->mBackup == 0)
			throw gcnew ObjectDisposedException("Instance of IList used after the object creating it was disposed.");

		UINT cComponent = 0;
		CheckCom(mBackupComponents->mBackup->GetWriterComponentsCount(&cComponent));
		return (int)cComponent;
	}

	VssWriterComponents^ VssBackupComponents::WriterComponentsList::default::get(int index)
	{
		if (index < 0 || index > Count)
			throw gcnew ArgumentOutOfRangeException("index");

		if (mBackupComponents->mBackup == 0)
			throw gcnew ObjectDisposedException("Instance of IList used after the object creating it was disposed.");

		IVssWriterComponentsExt *pWriterComponents;
		CheckCom(mBackupComponents->mBackup->GetWriterComponents(index, &pWriterComponents));
		return VssWriterComponents::Adopt(pWriterComponents);
	}

	VssBackupComponents::WriterMetadataList::WriterMetadataList(VssBackupComponents^ backupComponents)
		: mBackupComponents(backupComponents)
	{
	}

	int VssBackupComponents::WriterMetadataList::Count::get()
	{
		if (mBackupComponents->mBackup == 0)
			throw gcnew ObjectDisposedException("Instance of IList used after the object creating it was disposed.");

		UINT iCount;
		CheckCom(mBackupComponents->mBackup->GetWriterMetadataCount(&iCount));
		return (int)iCount;
	}

	VssExamineWriterMetadata^ VssBackupComponents::WriterMetadataList::default::get(int index)
	{
		if (index < 0 || index > Count)
			throw gcnew ArgumentOutOfRangeException("index");

		if (mBackupComponents->mBackup == 0)
			throw gcnew ObjectDisposedException("Instance of IList used after the object creating it was disposed.");

		VSS_ID idWriterInstance;
		IVssExamineWriterMetadata *ewm;
		CheckCom(mBackupComponents->mBackup->GetWriterMetadata(index, &idWriterInstance, &ewm));
		return VssExamineWriterMetadata::Adopt(ewm);
	}

	IList<VssExamineWriterMetadata^>^ VssBackupComponents::WriterMetadata::get()
	{
		return mWriterMetadata;
	}

	IList<VssWriterComponents^>^ VssBackupComponents::WriterComponents::get()
	{
		return mWriterComponents;
	}

	IList<VssWriterStatus^>^ VssBackupComponents::WriterStatus::get()
	{
		return mWriterStatus;
	}

	VssAsync^ VssBackupComponents::ImportSnapshots()
	{
		IVssAsync *pAsync;
		CheckCom(mBackup->ImportSnapshots(&pAsync));
		return VssAsync::Adopt(pAsync);
	}

	void VssBackupComponents::InitializeForBackup(String^ xml)
	{
		CheckCom(mBackup->InitializeForBackup(AutoMBStr(xml)));
	}

	void VssBackupComponents::InitializeForRestore(String^ xml)
	{
		CheckCom(mBackup->InitializeForRestore(NoNullAutoMBStr(xml)));
	}

	bool VssBackupComponents::IsVolumeSupported(Guid providerId, String^ volumeName)
	{
		BOOL eSupported;
		CheckCom(mBackup->IsVolumeSupported(ToVssId(providerId), NoNullAutoMBStr(volumeName), &eSupported));
		return (eSupported != 0);
	}

	VssAsync^ VssBackupComponents::PostRestore()
	{
		IVssAsync *pAsync;
		CheckCom(mBackup->PostRestore(&pAsync));
		return VssAsync::Adopt(pAsync);
	}

	VssAsync^ VssBackupComponents::PrepareForBackup()
	{
		IVssAsync *pAsync;
		CheckCom(mBackup->PrepareForBackup(&pAsync));
		return VssAsync::Adopt(pAsync);
	}

	VssAsync^ VssBackupComponents::PreRestore()
	{
		IVssAsync *pAsync;
		CheckCom(mBackup->PreRestore(&pAsync));
		return VssAsync::Adopt(pAsync);
	}

	IEnumerable<VssSnapshotProperties ^>^ VssBackupComponents::QuerySnapshots()
	{
		IVssEnumObject *pEnum;
		VSS_OBJECT_PROP rgelt;
		ULONG celtFetched = 0;
		IList<VssSnapshotProperties^> ^list = gcnew List<VssSnapshotProperties^>();

		CheckCom(mBackup->Query(GUID_NULL, VSS_OBJECT_NONE, VSS_OBJECT_SNAPSHOT, &pEnum));

		try
		{
			while (true)
			{
				CheckCom(pEnum->Next(1, &rgelt, &celtFetched));
				
				if (celtFetched == 0)
					return list;

				// Should always be snapshot, but just in case it isn't, we simply skip it.
				if (rgelt.Type == VSS_OBJECT_SNAPSHOT)
				{
					list->Add(VssSnapshotProperties::Adopt(&rgelt.Obj.Snap));
				}
			}
		}
		finally
		{
			pEnum->Release();
		}
	}

	IEnumerable<VssProviderProperties ^>^ VssBackupComponents::QueryProviders()
	{
		IVssEnumObject *pEnum;
		VSS_OBJECT_PROP rgelt;
		ULONG celtFetched = 0;
		IList<VssProviderProperties^> ^list = gcnew List<VssProviderProperties^>();

		CheckCom(mBackup->Query(GUID_NULL, VSS_OBJECT_NONE, VSS_OBJECT_PROVIDER, &pEnum));

		try
		{
			while (true)
			{
				CheckCom(pEnum->Next(1, &rgelt, &celtFetched));
				
				if (celtFetched == 0)
					return list;

				// Should always be a provider, but just in case it isn't, we simply skip it.
				if (rgelt.Type == VSS_OBJECT_PROVIDER)
				{
					list->Add(VssProviderProperties::Adopt(&rgelt.Obj.Prov));
				}
			}
		}
		finally
		{
			pEnum->Release();
		}
	}


	VssAsync^ VssBackupComponents::QueryRevertStatus(String^ volume)
	{
#if ALPHAVSS_TARGET == ALPHAVSS_TARGET_WIN2003 || ALPHAVSS_TARGET == ALPHAVSS_TARGET_WIN2008
		OsInfo::RequireAtLeastInFamily(OsVersion::Win2003SP1, OsVersion::Win2008);
		IVssAsync *pAsync;
		CheckCom(mBackup->QueryRevertStatus(NoNullAutoMStr(volume), &pAsync));
		return VssAsync::Adopt(pAsync);
#else
		UnsupportedOs();
#endif
	}

	void VssBackupComponents::RevertToSnapshot(Guid snapshotId, bool forceDismount)
	{
#if ALPHAVSS_TARGET == ALPHAVSS_TARGET_WIN2003 || ALPHAVSS_TARGET == ALPHAVSS_TARGET_WIN2008
		OsInfo::RequireAtLeastInFamily(OsVersion::Win2003SP1, OsVersion::Win2008);		
		CheckCom(mBackup->RevertToSnapshot(ToVssId(snapshotId), forceDismount));
#else
		UnsupportedOs();
#endif
	}

	String^ VssBackupComponents::SaveAsXml()
	{
		AutoBStr bstrXML;
		CheckCom(mBackup->SaveAsXML(&bstrXML));
		return bstrXML;
	}

	void VssBackupComponents::SetAdditionalRestores(Guid writerId, VssComponentType componentType, String^ logicalPath, String^ componentName, bool additionalResources)
	{
		CheckCom(mBackup->SetAdditionalRestores(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, AutoMStr(logicalPath), NoNullAutoMStr(componentName), additionalResources));
	}

	void VssBackupComponents::SetBackupOptions(Guid writerId, VssComponentType componentType, String^ logicalPath, String^ componentName, String^ backupOptions)
	{
		CheckCom(mBackup->SetBackupOptions(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, AutoMStr(logicalPath), NoNullAutoMStr(componentName), NoNullAutoMStr(backupOptions)));
	}

	void VssBackupComponents::SetBackupState(bool selectComponents, bool backupBootableSystemState, VssBackupType backupType, bool partialFileSupport)
	{
		CheckCom(mBackup->SetBackupState(selectComponents, backupBootableSystemState, (VSS_BACKUP_TYPE)backupType, partialFileSupport));
	}

	void VssBackupComponents::SetBackupSucceeded(Guid instanceId, Guid writerId, VssComponentType componentType, String^ logicalPath, String^ componentName, bool succeeded)
	{
		CheckCom(mBackup->SetBackupSucceeded(ToVssId(instanceId), ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, AutoMStr(logicalPath), NoNullAutoMStr(componentName), succeeded));
	}

	void VssBackupComponents::SetContext(VssVolumeSnapshotAttributes context)
	{
		CheckCom(mBackup->SetContext((LONG)context));
	}

	void VssBackupComponents::SetFileRestoreStatus(Guid writerId, VssComponentType componentType, String^ logicalPath, String^ componentName, VssFileRestoreStatus status)
	{
		CheckCom(mBackup->SetFileRestoreStatus(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, AutoMStr(logicalPath), NoNullAutoMStr(componentName), (VSS_FILE_RESTORE_STATUS)status));
	}

	void VssBackupComponents::SetPreviousBackupStamp(Guid writerId, VssComponentType componentType, String^ logicalPath, String^ componentName, String^ previousBackupStamp)
	{
		CheckCom(mBackup->SetPreviousBackupStamp(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, AutoMStr(logicalPath), NoNullAutoMStr(componentName), NoNullAutoMStr(previousBackupStamp)));
	}

	void VssBackupComponents::SetRangesFilePath(Guid writerId, VssComponentType componentType, String^ logicalPath, String^ componentName, int partialFileIndex, String^ rangesFile)
	{
#if ALPHAVSS_TARGET >= ALPHAVSS_TARGET_WIN2003
		OsInfo::RequireAtLeast(OsVersion::Win2003);		
		CheckCom(mBackup->SetRangesFilePath(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, AutoMStr(logicalPath), NoNullAutoMStr(componentName), partialFileIndex, NoNullAutoMStr(rangesFile)));
#else
		UnsupportedOs();
#endif
	}

	void VssBackupComponents::SetRestoreOptions(Guid writerId, VssComponentType componentType, String^ logicalPath, String^ componentName, String^ restoreOptions)
	{
		CheckCom(mBackup->SetRestoreOptions(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, AutoMStr(logicalPath), NoNullAutoMStr(componentName), NoNullAutoMStr(restoreOptions)));
	}

	void VssBackupComponents::SetRestoreState(VssRestoreType restoreType)
	{
#if ALPHAVSS_TARGET >= ALPHAVSS_TARGET_WIN2003
		OsInfo::RequireAtLeast(OsVersion::Win2003);		
		CheckCom(mBackup->SetRestoreState((VSS_RESTORE_TYPE)restoreType));
#else
		UnsupportedOs();
#endif
	}

	void VssBackupComponents::SetSelectedForRestore(Guid writerId, VssComponentType componentType, String^ logicalPath, String^ componentName, bool selectedForRestore)
	{
		CheckCom(mBackup->SetSelectedForRestore(ToVssId(writerId), (VSS_COMPONENT_TYPE)componentType, AutoMStr(logicalPath), NoNullAutoMStr(componentName), selectedForRestore));
	}

	Guid VssBackupComponents::StartSnapshotSet()
	{
		VSS_ID snapshotSetId;
		CheckCom(mBackup->StartSnapshotSet(&snapshotSetId));
		return ToGuid(snapshotSetId);
	}
}
} }