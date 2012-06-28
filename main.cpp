#pragma comment(lib, "node")

#using <mscorlib.dll>
#using <System.dll>
#using <System.Data.dll>
#using <System.Xml.dll>

#include <node.h>
#include <v8.h>
#include <string>
#include <gcroot.h>
#include <string>
#include <iostream>
#include <uv.h>

using namespace node;
using namespace v8;

class SQLConnection : ObjectWrap
{
private:
    //the field that will hold an instance of System.Diagnostics.EventLog
	gcroot<System::Data::SqlClient::SqlConnection^> _sqlConnection;

public:
    
    static Persistent<FunctionTemplate> s_ct;
    
    //module initialization
    static void NODE_EXTERN Init(Handle<Object> target)
    {
        HandleScope scope;

        // set the constructor function
        Local<FunctionTemplate> t = FunctionTemplate::New(New);

        // set the node.js/v8 class name
        s_ct = Persistent<FunctionTemplate>::New(t);
        s_ct->InstanceTemplate()->SetInternalFieldCount(1);
        s_ct->SetClassName(String::NewSymbol("SQLConnection"));

        // registers a class member functions 
        NODE_SET_PROTOTYPE_METHOD(s_ct, "query", Query);
        
        target->Set(String::NewSymbol("SQLConnection"),
            s_ct->GetFunction());
    }
    
    //Constructor
    //connString: The SQL Connection String to use to connect to the server with
    SQLConnection(System::String^ connString) 
    {
		_sqlConnection = gcnew System::Data::SqlClient::SqlConnection();
		_sqlConnection->ConnectionString = connString;
		_sqlConnection->Open();
    }

    //finalizer, kill the _eventLog.
    //This will call the IDisposable
    ~SQLConnection()
    {
        _sqlConnection->Close();
		delete _sqlConnection;
    }

    //Transform a v8 argument to a String
    static inline gcroot<System::String^> ParseArgument(Arguments const&args, int argumentIndex)
    {
        Local<String> message = Local<String>::Cast(args[argumentIndex]);
        gcroot<System::String^> m = gcnew System::String(((std::string)*v8::String::AsciiValue(message)).c_str());
        return m;
    }

    //This is the method that Node will call when we new is executed
    static Handle<Value> New(const Arguments& args)
    {
        if (!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
                String::New("First argument must be the SQL Connection String.")));
        }

        try{
			HandleScope scope;
	        
			System::String^ connString = ParseArgument(args, 0);

			SQLConnection* pm = new SQLConnection(connString);
			pm->Wrap(args.This());
			return args.This();
		}catch (System::Exception^ e){
			char* err = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(e->Message);
			return ThrowException(Exception::Error(String::New(err)));
		}catch (System::Data::SqlClient::SqlException^ e){
			char* err = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(e->Message);
			return ThrowException(Exception::Error(String::New(err)));
		}catch(char* str){
            return ThrowException(Exception::Error(String::New(str)));
		}
    }

	static Handle<Value> Query(const Arguments& args){
		if (!args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
				String::New("First argument must be the SQL Query to execute.")));
		}

		try{
			gcroot<System::String^> query = ParseArgument(args, 0);
			System::Data::SqlClient::SqlDataAdapter^ da;

			SQLConnection* xthis = ObjectWrap::Unwrap<SQLConnection>(args.This());
			da = gcnew System::Data::SqlClient::SqlDataAdapter(query, xthis->_sqlConnection);
			System::Data::DataSet^ resultsDataSet = gcnew System::Data::DataSet();
			da->Fill(resultsDataSet, "results");

			System::IO::StringWriter^ xmlWriter = gcnew System::IO::StringWriter;
			resultsDataSet->WriteXml(xmlWriter);

			HandleScope scope;
			char* str1 = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(xmlWriter->ToString());
			return scope.Close(String::New(str1));
		}catch (System::Exception^ e){
			char* err = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(e->Message);
			return ThrowException(Exception::Error(String::New(err)));
		}catch (System::Data::SqlClient::SqlException^ e){
			char* err = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(e->Message);
			return ThrowException(Exception::Error(String::New(err)));
		}catch(char* str){
            return ThrowException(Exception::Error(String::New(str)));
		}
	}
};

Persistent<FunctionTemplate> SQLConnection::s_ct;

extern "C" {
    void NODE_EXTERN init (Handle<Object> target)
    {
        SQLConnection::Init(target);
    }
    NODE_MODULE(sqlnative2, init);
}