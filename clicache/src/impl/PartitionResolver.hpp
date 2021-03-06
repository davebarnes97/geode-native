/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once


#include "../IPartitionResolver.hpp"
#include "../Region.hpp"
#include "SafeConvert.hpp"

using namespace System;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {

      public interface class IPartitionResolverProxy
      {
      public:
        std::shared_ptr<apache::geode::client::CacheableKey> getRoutingObject(const apache::geode::client::EntryEvent& ev);
        const std::string& getName();
      };

      generic<class TKey, class TValue>
      public ref class PartitionResolverGeneric : IPartitionResolverProxy
      {
        private:

          IPartitionResolver<TKey, TValue>^ m_resolver;

        public:

          void SetPartitionResolver(IPartitionResolver<TKey, TValue>^ resolver)
          {
            m_resolver = resolver;
          }

          virtual std::shared_ptr<apache::geode::client::CacheableKey> getRoutingObject(const apache::geode::client::EntryEvent& ev)
          {
            EntryEvent<TKey, TValue> gevent(&ev);
						Object^ groutingobject = m_resolver->GetRoutingObject(%gevent);
            return Serializable::GetUnmanagedValueGeneric<Object^>(groutingobject);
          }

          virtual const std::string& getName()
          {
            static const std::string name = marshal_as<std::string>(m_resolver->GetName());
            return name;
          }
      };
    }  // namespace Client
  }  // namespace Geode
}  // namespace Apache

