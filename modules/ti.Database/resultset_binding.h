/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _DATABASE_RESULTSET_BINDING_H_
#define _DATABASE_RESULTSET_BINDING_H_

#include <kroll/kroll.h>
#include "db_session.h"

namespace ti
{
	/**
	 * Binding for the ResultSet (or in Poco SQLite the RecordSet)
	 *
	 */
	class ResultSetBinding : public StaticBoundObject
	{
	public:
		ResultSetBinding();
		ResultSetBinding(Poco::Data::RecordSet &rs);
	protected:
		virtual ~ResultSetBinding();
	private:
		SharedPtr<Poco::Data::RecordSet> rs;
		bool eof;
		
		void Bind();
		void TransformValue(size_t index, SharedValue result);
		

		void IsValidRow(const ValueList& args, SharedValue result);
		void Next(const ValueList& args, SharedValue result);
		void Close(const ValueList& args, SharedValue result);
		void RowCount(const ValueList& args, SharedValue result);
		void FieldCount(const ValueList& args, SharedValue result);
		void FieldName(const ValueList& args, SharedValue result);
		void Field(const ValueList& args, SharedValue result);
		void FieldByName(const ValueList& args, SharedValue result);
	};
}

#endif
