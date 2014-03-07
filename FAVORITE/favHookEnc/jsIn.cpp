#include <v8.h>

#include <windows.h>
#include "favhook.h"

using namespace v8;

void myFread(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if(args.Length()!=2)
    {
        args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(),"Bad parameters"));
        return;
    }
    HANDLE hf=(HANDLE)(args[0]->Int32Value());
    int size=args[1]->Int32Value();

    auto buff=new BYTE[size];
    if(!buff)
    {
        args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(),"Not enough memory"));
        return;
    }
    DWORD temp=0;
    ReadFile(hf,buff,size,&temp,0);
    auto str=v8::String::NewFromOneByte(args.GetIsolate(),buff,v8::String::kNormalString,temp);
    delete[] buff;
    args.GetReturnValue().Set(str);
}

void myMread(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if(args.Length()!=2)
    {
        args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(),"Bad parameters"));
        return;
    }
    int start=args[0]->Int32Value();
    int size=args[1]->Int32Value();

    //auto checkMem=[start,size](){
    //    BYTE* p=(BYTE*)start;
    //    for(int i=0;i<size;i++)
    //    {
    //        char c;
    //        *(BYTE*)(&c)=*(BYTE*)(&p[i]);
    //    }
    //};
    //checkMem();

    auto str=v8::String::NewFromOneByte(args.GetIsolate(),(BYTE*)start,v8::String::kNormalString,size);
    args.GetReturnValue().Set(str);
}

#ifdef DBG1
void myMBox(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    HandleScope scope(args.GetIsolate());

    String::Value str(args[0]);
    MessageBox(0,(LPWSTR)*str,0,0);
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        if (first) {
            first = false;
        } else {
            printf(" ");
        }
        v8::String::Utf8Value str(args[i]);
        //const char* cstr = ToCString(str);
        printf("%s", *str);
    }
    printf("\n");
    fflush(stdout);
}
#endif

Handle<Context> InitV8()
{
    Isolate* isolate=Isolate::GetCurrent();


    Handle<ObjectTemplate> global = ObjectTemplate::New(isolate);
    global->Set(v8::String::NewFromUtf8(isolate, "fread"),
        v8::FunctionTemplate::New(isolate, myFread));
    global->Set(v8::String::NewFromUtf8(isolate, "mread"),
        v8::FunctionTemplate::New(isolate, myMread));

#ifdef DBG1
    global->Set(v8::String::NewFromUtf8(isolate, "mbox"),
        v8::FunctionTemplate::New(isolate, myMBox));
    global->Set(v8::String::NewFromUtf8(isolate, "print"),
        v8::FunctionTemplate::New(isolate, Print));

#endif // DBG1
    return Context::New(isolate,NULL,global);

}

