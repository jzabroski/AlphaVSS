﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alphaleonis.Win32.Vss
{
    /// <summary>
    /// The <see cref="VssDiffVolumeProperties"/> structure describes a shadow copy storage area volume.
    /// </summary>
    public class VssDiffVolumeProperties : IVssManagementObjectProperties
    {
        /// <summary>
        /// Gets the type of this management object properties instance.
        /// </summary>
        /// <value>The type HEREof this management HEREobject properties instance.</value>
        public VssManagementObjectType  Type { get { return VssManagementObjectType.DiffVolume; } }

        /// <summary>
        /// Initializes a new instance of the <see cref="VssDiffVolumeProperties"/> class.
        /// </summary>
        /// <param name="volumeName">Name of the volume.</param>
        /// <param name="volumeDisplayName">Display name of the volume.</param>
        /// <param name="volumeFreeSpace">The volume free space.</param>
        /// <param name="volumeTotalSpace">The volume total space.</param>
        public VssDiffVolumeProperties(string volumeName, string volumeDisplayName, Int64 volumeFreeSpace, Int64 volumeTotalSpace)
        {
            mVolumeName = volumeName;
            mVolumeDisplayName = volumeDisplayName;
            mVolumeFreeSpace = volumeFreeSpace;
            mVolumeTotalSpace = volumeTotalSpace;
        }

        /// <summary>
        /// Gets the shadow copy storage area volume name, in <c>\\?\Volume{GUID}\</c> format.
        /// </summary>
        /// <value>The shadow copy storage area volume name, in <c>\\?\Volume{GUID}\</c> format.</value>
        public string VolumeName { get { return mVolumeName; } }

        /// <summary>
        /// Gets a string that can be displayed to a user, for example <c>C:\</c>, for the shadow copy storage area volume.
        /// </summary>
        /// <value>A string that can be displayed to a user, for example <c>C:\</c>, for the shadow copy storage area volume.</value>
        public string VolumeDisplayName { get { return mVolumeDisplayName; } }

        /// <summary>
        /// Gets the free space, in bytes, on the shadow copy storage area volume.
        /// </summary>
        /// <value>The free space, in bytes, on the shadow copy storage area volume.</value>
        public Int64 VolumeFreeSpace { get { return mVolumeFreeSpace; } }

        /// <summary>
        /// Gets the total space, in bytes, on the shadow copy storage area volume.
        /// </summary>
        /// <value>The total space, in bytes, on the shadow copy storage area volume..</value>
        public Int64 VolumeTotalSpace { get { return mVolumeTotalSpace; } }

        private string mVolumeName;
        private string mVolumeDisplayName;
        private Int64 mVolumeFreeSpace;
        private Int64 mVolumeTotalSpace;

    }
}