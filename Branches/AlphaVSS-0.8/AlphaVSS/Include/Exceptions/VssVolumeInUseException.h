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

using namespace System;

#include "Exceptions/VssException.h"

namespace Alphaleonis { namespace Win32 { namespace Vss
{
	/// <summary>Exception indicating that the volume was in use and could not be locked.</summary>
	[Serializable]
	public ref class VssVolumeInUseException : VssException
	{
	public:
		/// <summary>Initializes a new instance of the <see cref="VssVolumeInUseException"/> class with a system-supplied message describing the error.</summary>
		VssVolumeInUseException();
		/// <summary>Initializes a new instance of the <see cref="VssVolumeInUseException"/> class with a specified error message.</summary>
		/// <param name="message">The message that describes the error</param>
		VssVolumeInUseException(String ^ message);
		/// <summary>Initializes a new instance of the <see cref="VssVolumeInUseException"/> class with a specified error message and a reference to the inner exception that is the cause of this exception.</summary>
		/// <param name="message">The message that describes the error</param>
		/// <param name="innerException">The exception that is the cause of the current exception.</param>
		VssVolumeInUseException(String ^ message, Exception ^ innerException);
	protected:		
		/// <summary>Initializes a new instance of the <see cref="VssVolumeInUseException"/> class with serialized data.</summary>
		/// <param name="info">The object that holds the serialized object data. </param>
		/// <param name="context">The contextual information about the source or destination.</param>
		VssVolumeInUseException(System::Runtime::Serialization::SerializationInfo ^ info, System::Runtime::Serialization::StreamingContext context);
	};
}
} }