/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ResultSet.h"

#include <Poco/Data/MetaColumn.h>
#include <Poco/DynamicAny.h>

using Poco::Data::MetaColumn;

namespace Titanium {

ResultSet::ResultSet() :
	StaticBoundObject("Database.ResultSet"),
	eof(true)
{
	// no results result set
	Bind();
}
ResultSet::ResultSet(Poco::Data::RecordSet &r) : StaticBoundObject("ResultSet"), eof(false)
{
	rs = new Poco::Data::RecordSet(r);
	Bind();
}
void ResultSet::Bind()
{
	/**
	 * @tiapi(method=True,name=Database.ResultSet.isValidRow,since=0.4) Checks whether you can call data extraction methods
	 * @tiresult(for=Database.ResultSet.isValidRow,type=Boolean) true if the row is valid
	 */
	this->SetMethod("isValidRow",&ResultSet::IsValidRow);

	/**
	 * @tiapi(method=True,name=Database.ResultSet.next,since=0.4) Moves the pointer to the next row of the result set
	 */
	this->SetMethod("next",&ResultSet::Next);

	/**
	 * @tiapi(method=True,name=Database.ResultSet.close,since=0.4) Releases the state associated with the result set
	 */
	this->SetMethod("close",&ResultSet::Close);

	/**
	 * @tiapi(method=True,name=Database.ResultSet.fieldCount,since=0.4) Returns the number of fields of the result set
	 * @tiresult(for=Database.ResultSet.fieldCount,type=Number) the number of fields of the result set
	 */
	this->SetMethod("fieldCount",&ResultSet::FieldCount);

	/**
	 * @tiapi(method=True,name=Database.ResultSet.rowCount,since=0.4) Returns the number of rows of the result set
	 * @tiresult(for=Database.ResultSet.rowCount,type=Number) the number of the rows of the result set
	 */
	this->SetMethod("rowCount",&ResultSet::RowCount);

	/**
	 * @tiapi(method=True,name=Database.ResultSet.fieldName,since=0.4) Returns the name of the specified field in the current result set taken from the SQL statement which was executed
	 * @tiarg(for=Database.ResultSet.fieldName,type=Number,name=fieldIndex) the zero-based index of the desired field
	 * @tiresult(for=Database.ResultSet.fieldName,type=String) The name of the specified field
	 */
	this->SetMethod("fieldName",&ResultSet::FieldName);

	/**
	 * @tiapi(method=True,name=Database.ResultSet.field,since=0.4) Returns the contents of the specified field in the current row
	 * @tiarg(for=Database.ResultSet.field,type=Number,name=fieldIndex) the zero-based index of the desired field
	 * @tiresult(for=Database.ResultSet.field,type=Boolean|String|Number|Bytes) The content of the specified field in the current row
	 */
	this->SetMethod("field",&ResultSet::Field);

	/**
	 * @tiapi(method=True,name=Database.ResultSet.fieldByName,since=0.4) Returns the contents of the specified field in the current row using the name of the field as an identifier
	 * @tiarg(for=Database.ResultSet.fieldByName,type=String,name=name) the name of the desired field
	 * @tiresult(for=Database.ResultSet.fieldByName,type=Boolean|String|Number|Bytes) The content of the specified field in the current row
	 */
	this->SetMethod("fieldByName",&ResultSet::FieldByName);
}
ResultSet::~ResultSet()
{
}
void ResultSet::IsValidRow(const ValueList& args, KValueRef result)
{
	if (rs.isNull())
	{
		result->SetBool(false);
	}
	else
	{
		result->SetBool(!eof);
	}
}
void ResultSet::Next(const ValueList& args, KValueRef result)
{
	if (!rs.isNull() && !eof)
	{
		eof = (rs->moveNext() == false);
	}
}
void ResultSet::Close(const ValueList& args, KValueRef result)
{
	if (!rs.isNull())
	{
		rs = NULL;
	}
}
void ResultSet::RowCount(const ValueList& args, KValueRef result)
{
	if (rs.isNull())
	{
		result->SetInt(0);
	}
	else
	{
		result->SetInt(rs->rowCount());
	}
}
void ResultSet::FieldCount(const ValueList& args, KValueRef result)
{
	if (rs.isNull())
	{
		result->SetInt(0);
	}
	else
	{
		result->SetInt(rs->columnCount());
	}
}
void ResultSet::FieldName(const ValueList& args, KValueRef result)
{
	if (rs.isNull())
	{
		result->SetNull();
	}
	else
	{
		args.VerifyException("fieldName", "i");
		const std::string &str = rs->columnName(args.at(0)->ToInt());
		result->SetString(str.c_str());
	}
}
void ResultSet::Field(const ValueList& args, KValueRef result)
{
	if (rs.isNull())
	{
		result->SetNull();
	}
	else
	{
		args.VerifyException("field", "i");
		TransformValue(args.at(0)->ToInt(),result);
	}
}
void ResultSet::FieldByName(const ValueList& args, KValueRef result)
{
	result->SetNull();
	if (!rs.isNull())
	{
		args.VerifyException("fieldByName", "s");
		std::string name = args.at(0)->ToString();
		size_t count = rs->columnCount();
		for (size_t i = 0; i<count; i++)
		{
			const std::string &str = rs->columnName(i);
			if (str == name)
			{
				TransformValue(i,result);
				break;
			}
		}
	}
}
void ResultSet::TransformValue(size_t index, KValueRef result)
{
	MetaColumn::ColumnDataType type = rs->columnType(index);
	Poco::DynamicAny value = rs->value(index);
	
	if (value.isEmpty())
	{
		result->SetNull();
	}
	else if (type == MetaColumn::FDT_STRING)
	{
		std::string str;
		value.convert(str);
		result->SetString(str);
	}
	else if (type == MetaColumn::FDT_BOOL)
	{
		bool v = false;
		value.convert(v);
		result->SetBool(v);
	}
	else if (type == MetaColumn::FDT_FLOAT || type == MetaColumn::FDT_DOUBLE)
	{
		float f = 0;
		value.convert(f);
		result->SetDouble(f);
	}
	else if (type == MetaColumn::FDT_BLOB || type == MetaColumn::FDT_UNKNOWN)
	{
		std::string str;
		value.convert(str);
		result->SetString(str);
	}
	else
	{
		// the rest of these are ints:
		// FDT_INT8,
		// FDT_UINT8,
		// FDT_INT16,
		// FDT_UINT16,
		// FDT_INT32,
		// FDT_UINT32,
		// FDT_INT64,
		// FDT_UINT64,
		int i;
		value.convert(i);
		result->SetInt(i);
	}
}

} // namespace Titanium
