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
		

		DECLAREBOUNDMETHOD(IsValidRow);
		DECLAREBOUNDMETHOD(Next);
		DECLAREBOUNDMETHOD(Close);
		DECLAREBOUNDMETHOD(RowCount);
		DECLAREBOUNDMETHOD(FieldCount);
		DECLAREBOUNDMETHOD(FieldName);
		DECLAREBOUNDMETHOD(Field);
		DECLAREBOUNDMETHOD(FieldByName);
	};
}

#endif
