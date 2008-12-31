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
#pragma once

#include "Config.h"

#ifdef ALPHAVSS_HAS_DIFFERENTIALSOFTWARESNAPSHOTMGMT

#include <vss.h>

#include <VsMgmt.h>
#include "Macros.h"

using namespace System::Collections::Generic;

namespace Alphaleonis { namespace Win32 { namespace Vss
{
	public ref class VssDifferentialSoftwareSnapshotManagement : IVssDifferentialSoftwareSnapshotManagement
	{
	public:
		~VssDifferentialSoftwareSnapshotManagement();
		!VssDifferentialSoftwareSnapshotManagement();

        virtual void AddDiffArea(String^ volumeName, String^ diffAreaVolumeName, Int64 maximumDiffSpace);
        virtual void ChangeDiffAreaMaximumSize(String^ volumeName, String^ diffAreaVolumeName, Int64 maximumDiffSpace);
        virtual IList<IVssManagementObjectProperties^>^ QueryDiffAreasForSnapshot(Guid snapshotId);
        virtual IList<IVssManagementObjectProperties^>^ QueryDiffAreasForVolume(String^ volumeName);
        virtual IList<IVssManagementObjectProperties^>^ QueryDiffAreasOnVolume(String^ volumeName);
        virtual IList<IVssManagementObjectProperties^>^ QueryVolumesSupportedForDiffAreas(String^ originalVolumeName);

        //
        // From IVssDifferentialSoftwareSnapshotMgmt2
        //
        virtual void ChangeDiffAreaMaximumSize(String^ volumeName, String^ diffAreaVolumeName, Int64 maximumDiffSpace, bool isVolatile);

        //
        // From IVssDifferentialSoftwareSnapshotMgmt3
        //
        virtual void ClearVolumeProtectFault(String^ volumeName);
        virtual void DeleteUnusedDiffAreas(String^ diffAreaVolumeName);
        virtual VssVolumeProtectionInfo^ GetVolumeProtectionLevel(String^ volumeName);
        virtual void SetVolumeProtectionLevel(String^ volumeName, VssProtectionLevel protectionLevel);

	internal:
		VssDifferentialSoftwareSnapshotManagement(::IVssDifferentialSoftwareSnapshotMgmt *pMgmt);
	private:
		static IList<IVssManagementObjectProperties^>^ CreateListFromEnumMgmtObject(IVssEnumMgmtObject *pEnum);

		::IVssDifferentialSoftwareSnapshotMgmt *mMgmt;
#ifdef ALPHAVSS_HAS_DIFFERENTIALSOFTWARESNAPSHOTMGMT2
		DEFINE_EX_INTERFACE_ACCESSOR(IVssDifferentialSoftwareSnapshotMgmt2, mMgmt);
#endif

#ifdef ALPHAVSS_HAS_DIFFERENTIALSOFTWARESNAPSHOTMGMT3
		DEFINE_EX_INTERFACE_ACCESSOR(IVssDifferentialSoftwareSnapshotMgmt3, mMgmt);
#endif

	};

}
}}

#endif