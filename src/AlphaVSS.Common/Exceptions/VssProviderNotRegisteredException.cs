
using System;
using System.Runtime.Serialization;

namespace Alphaleonis.Win32.Vss
{
   /// <summary>
   ///     Exception thrown to indicate that the requested identifier does not correspond to a registered provider.
   /// </summary>
   [Serializable]
   public sealed class VssProviderNotRegisteredException : VssException
   {
      /// <summary>
      ///     Initializes a new instance of the <see cref="VssProviderNotRegisteredException"/> 
      ///     class with a system-supplied message describing the error.
      /// </summary>
      public VssProviderNotRegisteredException()
         : base(Resources.LocalizedStrings.TheRequestedIdentifierDoesNotCorrespondToARegisteredProvider)
      {
      }

      /// <summary>
      ///     Initializes a new instance of the <see cref="VssProviderNotRegisteredException"/> class with a specified error message.
      /// </summary>
      /// <param name="message">The message that describes the error</param>
      public VssProviderNotRegisteredException(string message)
         : base(message)
      {
      }

      /// <summary>
      ///     Initializes a new instance of the <see cref="VssProviderNotRegisteredException"/> class with 
      ///     a specified error message and a reference to the inner exception that is the cause of this exception.
      /// </summary>
      /// <param name="message">The message that describes the error</param>
      /// <param name="innerException">The exception that is the cause of the current exception.</param>
      public VssProviderNotRegisteredException(string message, Exception innerException)
         : base(message, innerException)
      {
      }

      /// <summary>
      /// Initializes a new instance of the <see cref="VssProviderNotRegisteredException"/> class.
      /// </summary>
      /// <param name="info">The <see cref="T:System.Runtime.Serialization.SerializationInfo"/> that holds the serialized object data about the exception being thrown.</param>
      /// <param name="context">The <see cref="T:System.Runtime.Serialization.StreamingContext"/> that contains contextual information about the source or destination.</param>
      /// <exception cref="T:System.ArgumentNullException">The <paramref name="info"/> parameter is <see langword="null"/>. </exception>
      /// <exception cref="T:System.Runtime.Serialization.SerializationException">The class name is <see langword="null"/> or <see cref="P:System.Exception.HResult"/> is zero (0). </exception>
      private VssProviderNotRegisteredException(SerializationInfo info, StreamingContext context)
         : base(info, context)
      {
      }
   }
}
