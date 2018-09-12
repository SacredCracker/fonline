#include "ScriptInvoker.h"
#include "Script.h"

#ifdef FONLINE_SERVER
# include "DataBase.h"
#endif

ScriptInvoker::ScriptInvoker()
{
    // Dummy
}

uint ScriptInvoker::AddDeferredCall( uint delay, bool saved, asIScriptFunction* func, int* value, CScriptArray* values )
{
    if( !func )
        SCRIPT_ERROR_R0( "Function arg is null." );

    RUNTIME_ASSERT( func->GetReturnTypeId() == asTYPEID_VOID );
    uint func_num = Script::GetFuncNum( func );
    uint bind_id = Script::BindByFunc( func, false );
    RUNTIME_ASSERT( bind_id );

    uint time_mul = Globals->GetTimeMultiplier();
    if( time_mul == 0 )
        time_mul = 1;

    DeferredCall call;
    call.Id = 0;
    call.FireTick = ( delay ? GameOpt.FullSecond + delay * 1000 / time_mul : 0 );
    call.FuncNum = func_num;
    call.BindId = bind_id;
    call.Saved = saved;

    if( value )
    {
        call.IsValue = true;
        call.Value = *value;
        int value_type_id = 0;
        func->GetParam( 0, &value_type_id );
        RUNTIME_ASSERT( value_type_id == asTYPEID_INT32 || value_type_id == asTYPEID_UINT32 );
        call.ValueSigned = ( value_type_id == asTYPEID_INT32 );
    }
    else
    {
        call.IsValue = false;
        call.Value = 0;
        call.ValueSigned = false;
    }

    if( values )
    {
        call.IsValues = true;
        Script::AssignScriptArrayInVector( call.Values, values );
        RUNTIME_ASSERT( values->GetElementTypeId() == asTYPEID_INT32 || values->GetElementTypeId() == asTYPEID_UINT32 );
        call.ValuesSigned = ( values->GetElementTypeId() == asTYPEID_INT32 );
    }
    else
    {
        call.IsValues = false;
        call.ValuesSigned = false;
    }

    if( delay == 0 )
    {
        RunDeferredCall( call );
    }
    else
    {
        RUNTIME_ASSERT( call.FireTick != 0 );
        call.Id = Globals->GetLastDeferredCallId() + 1;
        Globals->SetLastDeferredCallId( call.Id );
        deferredCalls.push_back( call );

        #ifdef FONLINE_SERVER
        if( call.Saved )
        {
            StrMap call_data;
            call_data[ "Script" ] = _str().parseHash( call.FuncNum );
            call_data[ "FireTick" ] = _str( "{}", call.FireTick );

            if( call.IsValue )
            {
                call_data[ "ValueSigned" ] = ( call.ValueSigned ? "true" : "false" );
                call_data[ "Value" ] = _str( "{}", call.Value );
            }

            if( call.IsValues )
            {
                call_data[ "ValuesSigned" ] = ( call.ValuesSigned ? "true" : "false" );
                string values;
                for( int v : call.Values )
                    values.append( _str( "{}", v ) ).append( " " );
                call_data[ "Values" ] = values;
            }

            DbStorage->Insert( "DeferredCalls", call.Id, call_data );
        }
        #endif
    }
    return call.Id;
}

bool ScriptInvoker::IsDeferredCallPending( uint id )
{
    for( auto it = deferredCalls.begin(); it != deferredCalls.end(); ++it )
        if( it->Id == id )
            return true;
    return false;
}

bool ScriptInvoker::CancelDeferredCall( uint id )
{
    for( auto it = deferredCalls.begin(); it != deferredCalls.end(); ++it )
    {
        if( it->Id == id )
        {
            deferredCalls.erase( it );

            #ifdef FONLINE_SERVER
            DbStorage->Delete( "DeferredCalls", id );
            #endif
            return true;
        }
    }
    return false;
}

bool ScriptInvoker::GetDeferredCallData( uint id, DeferredCall& data )
{
    for( auto it = deferredCalls.begin(); it != deferredCalls.end(); ++it )
    {
        DeferredCall& call = *it;
        if( call.Id == id )
        {
            data = call;
            return true;
        }
    }
    return false;
}

void ScriptInvoker::GetDeferredCallsList( IntVec& ids )
{
    ids.reserve( deferredCalls.size() );
    for( auto it = deferredCalls.begin(); it != deferredCalls.end(); ++it )
        ids.push_back( it->Id );
}

void ScriptInvoker::Process()
{
    bool done = false;
    while( !done )
    {
        done = true;

        for( auto it = deferredCalls.begin(); it != deferredCalls.end(); ++it )
        {
            RUNTIME_ASSERT( it->FireTick != 0 );
            if( GameOpt.FullSecond >= it->FireTick )
            {
                DeferredCall call = *it;
                it = deferredCalls.erase( it );

                #ifdef FONLINE_SERVER
                DbStorage->Delete( "DeferredCalls", call.Id );
                #endif

                RunDeferredCall( call );
                done = false;
                break;
            }
        }
    }
}

void ScriptInvoker::RunDeferredCall( DeferredCall& call )
{
    Script::PrepareContext( call.BindId, "Invoker" );

    CScriptArray* arr = nullptr;

    if( call.IsValue )
    {
        Script::SetArgUInt( call.Value );
    }
    else if( call.IsValues )
    {
        arr = Script::CreateArray( call.ValuesSigned ? "int[]" : "uint[]" );
        Script::AppendVectorToArray( call.Values, arr );
        Script::SetArgObject( arr );
    }

    if( call.FireTick == 0 )
        Script::RunPreparedSuspend();
    else
        Script::RunPrepared();

    if( arr )
        arr->Release();
}

string ScriptInvoker::GetStatistics()
{
    uint   time_mul = Globals->GetTimeMultiplier();
    string result = _str( "Deferred calls count: {}\n", (uint) deferredCalls.size() );
    result += "Id         Delay      Saved    Function                                                              Values\n";
    for( auto it = deferredCalls.begin(); it != deferredCalls.end(); ++it )
    {
        DeferredCall& call = *it;
        string        func_name = Script::GetBindFuncName( call.BindId );
        uint          delay = call.FireTick > GameOpt.FullSecond ? ( call.FireTick - GameOpt.FullSecond ) * 1000 / time_mul : 0;

        result += _str( "{:<10} {:<10} {:<8} {:<70}", call.Id, delay, call.Saved ? "true" : "false", func_name.c_str() );

        if( call.IsValue )
        {
            result += "Single:";
            result += _str( " {}", call.Value );
        }
        else if( call.IsValues )
        {
            result += "Multiple:";
            for( size_t i = 0; i < call.Values.size(); i++ )
                result += _str( " {}", call.Values[ i ] );
        }
        else
        {
            result += "None";
        }

        result += "\n";
    }
    return result;
}

#ifdef FONLINE_SERVER
bool ScriptInvoker::LoadDeferredCalls()
{
    WriteLog( "Load deferred calls...\n" );

    UIntVec call_ids = DbStorage->GetAllIds( "DeferredCalls" );
    int     errors = 0;
    for( uint call_id : call_ids )
    {
        StrMap       call_data = DbStorage->Get( "DeferredCalls", call_id );

        DeferredCall call;
        call.Id = _str( call_data[ "Id" ] ).toUInt();
        call.FireTick = _str( call_data[ "FireTick" ] ).toUInt();
        RUNTIME_ASSERT( call.FireTick != 0 );

        call.IsValue = ( call_data.count( "Value" ) > 0 );
        if( call.IsValue )
        {
            call.ValueSigned = _str( call_data[ "ValueSigned" ] ).compareIgnoreCase( "true" );
            call.Value = _str( call_data[ "Value" ] ).toInt();
        }
        else
        {
            call.ValueSigned = false;
            call.Value = 0;
        }

        call.IsValues = ( call_data.count( "Values" ) > 0 );
        if( call.IsValues )
        {
            call.ValuesSigned = _str( call_data[ "ValuesSigned" ] ).compareIgnoreCase( "true" );
            call.Values = _str( call_data[ "Values" ] ).splitToInt( ' ' );
        }
        else
        {
            call.ValuesSigned = false;
            call.Values.clear();
        }

        if( call.IsValue && call.IsValues )
        {
            WriteLog( "Deferred call {} have value and values.\n", call.Id );
            errors++;
            continue;
        }

        const char* decl;
        if( call.IsValue && call.ValueSigned )
            decl = "void %s(int)";
        else if( call.IsValue && !call.ValueSigned )
            decl = "void %s(uint)";
        else if( call.IsValues && call.ValuesSigned )
            decl = "void %s(int[]&)";
        else if( call.IsValues && !call.ValuesSigned )
            decl = "void %s(uint[]&)";
        else
            decl = "void %s()";

        call.FuncNum = Script::BindScriptFuncNumByFuncName( call_data[ "Script" ].c_str(), decl );
        if( !call.FuncNum )
        {
            WriteLog( "Unable to find function '{}' with declaration '{}' for deferred call {}.\n", call_data[ "Script" ].c_str(), decl, call.Id );
            errors++;
            continue;
        }

        call.BindId = Script::BindByFuncNum( call.FuncNum, false );
        if( !call.BindId )
        {
            WriteLog( "Unable to bind script function '{}' for deferred call {}.\n", _str().parseHash( call.FuncNum ), call.Id );
            errors++;
            continue;
        }

        call.Saved = true;
        deferredCalls.push_back( call );
    }

    WriteLog( "Load deferred calls complete, count {}.\n", (uint) deferredCalls.size() );
    return errors == 0;
}
#endif

uint ScriptInvoker::Global_DeferredCall( uint delay, asIScriptFunction* func )
{
    #pragma MESSAGE( "Take Invoker from func." )
    return Script::GetInvoker()->AddDeferredCall( delay, false, func, nullptr, nullptr );
}

uint ScriptInvoker::Global_DeferredCallWithValue( uint delay, asIScriptFunction* func, int value )
{
    return Script::GetInvoker()->AddDeferredCall( delay, false, func, &value, nullptr );
}

uint ScriptInvoker::Global_DeferredCallWithValues( uint delay, asIScriptFunction* func, CScriptArray* values )
{
    return Script::GetInvoker()->AddDeferredCall( delay, false, func, nullptr, values );
}

uint ScriptInvoker::Global_SavedDeferredCall( uint delay, asIScriptFunction* func )
{
    return Script::GetInvoker()->AddDeferredCall( delay, true, func, nullptr, nullptr );
}

uint ScriptInvoker::Global_SavedDeferredCallWithValue( uint delay, asIScriptFunction* func, int value )
{
    return Script::GetInvoker()->AddDeferredCall( delay, true, func, &value, nullptr );
}

uint ScriptInvoker::Global_SavedDeferredCallWithValues( uint delay, asIScriptFunction* func, CScriptArray* values )
{
    return Script::GetInvoker()->AddDeferredCall( delay, true, func, nullptr, values );
}

bool ScriptInvoker::Global_IsDeferredCallPending( uint id )
{
    return Script::GetInvoker()->IsDeferredCallPending( id );
}

bool ScriptInvoker::Global_CancelDeferredCall( uint id )
{
    return Script::GetInvoker()->CancelDeferredCall( id );
}

bool ScriptInvoker::Global_GetDeferredCallData( uint id, uint& delay, CScriptArray* values )
{
    ScriptInvoker* self = Script::GetInvoker();
    DeferredCall   call;
    if( self->GetDeferredCallData( id, call ) )
    {
        delay = ( call.FireTick > GameOpt.FullSecond ? ( call.FireTick - GameOpt.FullSecond ) * 1000 / Globals->GetTimeMultiplier() : 0 );
        if( values )
        {
            if( call.IsValue )
            {
                values->Resize( 1 );
                *(int*) values->At( 0 ) = call.Value;
            }
            else if( call.IsValues )
            {
                values->Resize( 0 );
                Script::AppendVectorToArray( call.Values, values );
            }
        }
        return true;
    }
    return false;
}

uint ScriptInvoker::Global_GetDeferredCallsList( CScriptArray* ids )
{
    ScriptInvoker* self = Script::GetInvoker();
    IntVec         ids_;
    self->GetDeferredCallsList( ids_ );
    if( ids )
        Script::AppendVectorToArray( ids_, ids );
    return (uint) ids_.size();
}
