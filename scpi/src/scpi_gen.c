/**
 * @brief Red Pitaya Scpi server generate SCPI commands implementation
 * @Author Red Pitaya
 * (c) Red Pitaya  http://www.redpitaya.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "scpi_gen.h"
#include "redpitaya/rp1.h"

#include "common.h"
#include "scpi/parser.h"
#include "scpi/units.h"
#include "scpi/error.h"

const scpi_choice_def_t rpscpi_gen_waveform_names[] = {
    {"SINusoid", 0},
    {"SQUare",   1},
    {"TRIangle", 2},
    {"USER",     7},
    SCPI_CHOICE_LIST_END
};

const scpi_choice_def_t rpscpi_gen_modes[] = {
    {"PERiodic", 0},
    {"BURSt",    1},
    SCPI_CHOICE_LIST_END
};

const scpi_choice_def_t rpscpi_gen_burst_modes[] = {
    {"FINite",   0},
    {"INFinite", 1},
    SCPI_CHOICE_LIST_END
};

//// These structure is a direct API mirror and should not be altered!
//const scpi_choice_def_t scpi_RpGenTrig[] = {
//    {"INT",     1},
//    {"EXT_PE",  2},
//    {"EXT_NE",  3},
//    {"GATED",   4},
//    SCPI_CHOICE_LIST_END
//};

static scpi_bool_t spcspi_gen_channels(scpi_t *context, int unsigned *channel) {
    int32_t num[1];
    rpscpi_context_t *rp = (rpscpi_context_t *) context->user_context;

    SCPI_CommandNumbers(context, num, 1, RPSCPI_CMD_NUM);
    if (!((num[0] > 0) && (num[0] <= rp->gen_num))) {
        SCPI_ErrorPush(context, SCPI_ERROR_HEADER_SUFFIX_OUTOFRANGE);
        return FALSE;
    }
    *channel = num[0] - 1;
    return TRUE;
}

scpi_result_t rpscpi_gen_reset(scpi_t *context) {
    int unsigned channel;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    rp_evn_reset(&gen[channel].evn);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_start(scpi_t *context) {
    int unsigned channel;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    rp_evn_start(&gen[channel].evn);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_stop(scpi_t *context) {
    int unsigned channel;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    rp_evn_stop(&gen[channel].evn);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_trigger(scpi_t *context) {
    int unsigned channel;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    rp_evn_trigger(&gen[channel].evn);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_mode(scpi_t *context) {
    int unsigned channel;
    int32_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if(!SCPI_ParamChoice(context, rpscpi_gen_modes, &value, true)){
        SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        return SCPI_RES_ERR;
    }
    rp_gen_set_mode(&gen[channel], value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_mode(scpi_t *context) {
    int unsigned channel;
    int32_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = (rp_gen_get_mode(&gen[channel]) >> 0) & 0x1;
    const char * text;
    if(!SCPI_ChoiceToName(rpscpi_gen_modes, value, &text)){
        return SCPI_RES_ERR;
    }
    SCPI_ResultMnemonic(context, text);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_burst_mode(scpi_t *context) {
    int unsigned channel;
    int32_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if(!SCPI_ParamChoice(context, rpscpi_gen_burst_modes, &value, true)){
        SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        return SCPI_RES_ERR;
    }
    value = (value << 1) | (rp_gen_get_mode(&gen[channel]) & 0x1);
    rp_gen_set_mode(&gen[channel], value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_burst_mode(scpi_t *context) {
    int unsigned channel;
    int32_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = (rp_gen_get_mode(&gen[channel]) >> 1) & 0x1;
    const char * text;
    if(!SCPI_ChoiceToName(rpscpi_gen_burst_modes, value, &text)){
        return SCPI_RES_ERR;
    }
    SCPI_ResultMnemonic(context, text);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_enable(scpi_t *context) {
    int unsigned channel;
    bool value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if(!SCPI_ParamBool(context, &value, true)){
        return SCPI_RES_ERR;
    }
    rp_gen_out_set_enable(&gen[channel].out, value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_enable(scpi_t *context) {
    int unsigned channel;
    bool value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = rp_gen_out_get_enable(&gen[channel].out);
    SCPI_ResultBool(context, value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_frequency(scpi_t *context) {
    int unsigned channel;
    scpi_number_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &value, true)) {
        return SCPI_RES_ERR;
    }
    if (value.special) {
        // special values are not allowed
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    } else if (rp_asg_gen_set_frequency(&gen[channel].per, value.content.value) != 0) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_frequency(scpi_t *context) {
    int unsigned channel;
    double value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = rp_asg_gen_get_frequency(&gen[channel].per);
    SCPI_ResultDouble(context, value);
    return SCPI_RES_OK;
}    

scpi_result_t rpscpi_gen_set_phase(scpi_t *context) {
    int unsigned channel;
    scpi_number_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;
    
    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &value, true)) {
        return SCPI_RES_ERR;
    }
    if (value.special) {
        // special values are not allowed
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    } else if (rp_asg_gen_set_phase(&gen[channel].per, value.content.value) != 0) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_phase(scpi_t *context) {
    int unsigned channel;
    double value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;
    
    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = rp_asg_gen_get_phase(&gen[channel].per);
    SCPI_ResultDouble(context, value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_amplitude(scpi_t *context) {
    int unsigned channel;
    scpi_number_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &value, true)) {
        return SCPI_RES_ERR;
    }
    if (value.special) {
        // special values are not allowed
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    } else if (rp_gen_out_set_amplitude(&gen[channel].out, (float) value.content.value)) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_amplitude(scpi_t *context) {
    int unsigned channel;
    float value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = rp_gen_out_get_amplitude(&gen[channel].out);
    SCPI_ResultFloat(context, value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_offset(scpi_t *context) {
    int unsigned channel;
    scpi_number_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &value, true)) {
        return SCPI_RES_ERR;
    }
    if (value.special) {
        // special values are not allowed
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    } else if (rp_gen_out_set_offset(&gen[channel].out, (float) value.content.value)) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_offset(scpi_t *context) {
    int unsigned channel;
    float value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = rp_gen_out_get_offset(&gen[channel].out);
    SCPI_ResultFloat(context, value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_waveform_tag(scpi_t *context) {
    int unsigned channel;
    rpscpi_context_t *user_context = (rpscpi_context_t *) context->user_context;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;
    float   *waveform     =  user_context->gen_waveform;
    int32_t *waveform_tag = &user_context->gen_waveform_tag;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if(!SCPI_ParamChoice(context, rpscpi_gen_waveform_names, waveform_tag, true)){
        SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        return SCPI_RES_ERR;
    }
    switch(*waveform_tag) {
        case 0:
            rp_wave_sin (waveform, gen[channel].buffer_size);
            break;
        case 1:
            rp_wave_squ (waveform, gen[channel].buffer_size, 0.5); // TODO 
            break;
        case 2:
            rp_wave_tri (waveform, gen[channel].buffer_size, 0.5); // TODO
            break;
        default:
            break;
    }
    if(rp_gen_set_waveform(&gen[channel], waveform, gen[channel].buffer_size)) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_waveform_tag(scpi_t *context) {
    int unsigned channel;
    rpscpi_context_t *user_context = (rpscpi_context_t *) context->user_context;
//    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;
//    float   *waveform     =  user_context->gen_waveform;
    int32_t *waveform_tag = &user_context->gen_waveform_tag;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    const char * text;
    if(!SCPI_ChoiceToName(rpscpi_gen_waveform_names, *waveform_tag, &text)){
        return SCPI_RES_ERR;
    }
    SCPI_ResultMnemonic(context, text);
    return SCPI_RES_OK;
}

//scpi_result_t RP_GenArbitraryWaveForm(scpi_t *context) {
//    
//    rp_channel_t channel;
//    float buffer[BUFFER_LENGTH];
//    uint32_t size;
//    int result;
//
//    if (!spcspi_gen_channels(context, &channel)) {
//        return SCPI_RES_ERR;
//    }
//
//    if(!SCPI_ParamBufferFloat(context, buffer, &size, true)){
//        RP_LOG(LOG_ERR, "*SOUR#:TRAC:DATA:DATA Failed to "
//            "arbitrary waveform data parameter.\n");
//        return SCPI_RES_ERR;
//    }
//
//    result = rp_GenArbWaveform(channel, buffer, size);
//    if(result != RP_OK){
//        RP_LOG(LOG_ERR, "*SOUR#:TRAC:DATA:DATA Failed to "
//            "set arbitrary waveform data: %s\n", rp_GetError(result));
//        return SCPI_RES_ERR;
//    }
//
//    RP_LOG(LOG_INFO, "*SOUR#:TRAC:DATA:DATA Successfully set arbitrary waveform data.\n");
//    return SCPI_RES_OK;
//}

//scpi_result_t RP_GenArbitraryWaveFormQ(scpi_t *context) {
//    
//    rp_channel_t channel;
//    float buffer[BUFFER_LENGTH];
//    uint32_t size;
//    int result;
//
//    if (!spcspi_gen_channels(context, &channel)) {
//        return SCPI_RES_ERR;
//    }
//
//    result = rp_GenGetArbWaveform(channel, buffer, &size);
//    if(result != RP_OK){
//        RP_LOG(LOG_ERR, "*SOUR#:TRAC:DATA:DATA? Failed to "
//            "get arbitrary waveform data: %s\n", rp_GetError(result));
//        return SCPI_RES_ERR;
//    }
//
//    SCPI_ResultBufferFloat(context, buffer, size);
//
//    RP_LOG(LOG_INFO, "*SOUR#:TRAC:DATA:DATA? Successfully "
//        "returned arbitrary waveform data to client.\n");
//    return SCPI_RES_OK;
//}

scpi_result_t rpscpi_gen_set_data_repetitions(scpi_t *context) {
    int unsigned channel;
    scpi_number_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &value, true)) {
        return SCPI_RES_ERR;
    }
    if (value.special) {
        // special values are not allowed
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    } else if (rp_asg_bst_set_data_repetitions(&gen[channel].bst, (int) value.content.value)) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_data_repetitions(scpi_t *context) {
    int unsigned channel;
    float value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = rp_asg_bst_get_data_repetitions(&gen[channel].bst);
    SCPI_ResultUInt32(context, value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_data_length(scpi_t *context) {
    int unsigned channel;
    scpi_number_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &value, true)) {
        return SCPI_RES_ERR;
    }
    if (value.special) {
        // special values are not allowed
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    } else if (rp_asg_bst_set_data_length(&gen[channel].bst, (int) value.content.value)) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_data_length(scpi_t *context) {
    int unsigned channel;
    float value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = rp_asg_bst_get_data_length(&gen[channel].bst);
    SCPI_ResultUInt32(context, value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_period_length(scpi_t *context) {
    int unsigned channel;
    scpi_number_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &value, true)) {
        return SCPI_RES_ERR;
    }
    if (value.special) {
        // special values are not allowed
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    } else if (rp_asg_bst_set_period_length(&gen[channel].bst, (int) value.content.value)) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_period_length(scpi_t *context) {
    int unsigned channel;
    float value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = rp_asg_bst_get_period_length(&gen[channel].bst);
    SCPI_ResultUInt32(context, value);
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_set_period_number(scpi_t *context) {
    int unsigned channel;
    scpi_number_t value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &value, true)) {
        return SCPI_RES_ERR;
    }
    if (value.special) {
        // special values are not allowed
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    } else if (rp_asg_bst_set_period_number(&gen[channel].bst, (int) value.content.value)) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

scpi_result_t rpscpi_gen_get_period_number(scpi_t *context) {
    int unsigned channel;
    float value;
    rp_gen_t *gen = ((rpscpi_context_t *) context->user_context)->gen;

    if (!spcspi_gen_channels(context, &channel)) {
        return SCPI_RES_ERR;
    }
    value = rp_asg_bst_get_period_number(&gen[channel].bst);
    SCPI_ResultUInt32(context, value);
    return SCPI_RES_OK;
}

//scpi_result_t RP_GenTriggerSource(scpi_t *context) {
//        
//    rp_channel_t channel;
//    int32_t trig_choice;
//    int result;
//
//    if (!spcspi_gen_channels(context, &channel)) {
//        return SCPI_RES_ERR;
//    }
//
//    if(!SCPI_ParamChoice(context, scpi_RpGenTrig, &trig_choice, true)){
//        RP_LOG(LOG_ERR, "*SOUR#:TRIG:SOUR Failed to parse first parameter.\n");
//        return SCPI_RES_ERR;
//    }
//
//    rp_trig_src_t trig_src = trig_choice;
//    result = rp_GenTriggerSource(channel, trig_src);
//    if(result != RP_OK){
//        RP_LOG(LOG_ERR, "*SOUR#:TRIG:SOUR Failed to set generate"
//        " trigger source: %s\n", rp_GetError(result));
//
//        return SCPI_RES_ERR;
//    }
//
//    RP_LOG(LOG_INFO, "*SOUR#:TRIG:SOUR Successfully set generate trigger source.\n");
//    return SCPI_RES_OK;
//}
//
//scpi_result_t RP_GenTriggerSourceQ(scpi_t *context) {
//    
//    rp_channel_t channel;
//    const char *trig_name;
//
//    if (!spcspi_gen_channels(context, &channel)) {
//        return SCPI_RES_ERR;
//    }
//
//    rp_trig_src_t trig_src;
//    if (rp_GenGetTriggerSource(channel, &trig_src) != RP_OK){
//        return SCPI_RES_ERR;
//    }
//
//    int32_t trig_n = trig_src;
//
//    if(!SCPI_ChoiceToName(scpi_RpGenTrig, trig_n, &trig_name)){
//        RP_LOG(LOG_ERR, "*SOUR#:TRIG:SOUR? Failed to parse trigger name.\n");
//        return SCPI_RES_ERR;
//    }
//
//    SCPI_ResultMnemonic(context, trig_name);
//
//    RP_LOG(LOG_INFO, "*SOUR#:TRIG:SOUR? Successfully returend"
//    " generate trigger status to client.\n");
//
//    return SCPI_RES_OK;
//}
//
//scpi_result_t RP_GenTrigger(scpi_t *context) {
//    
//    rp_channel_t channel;
//    int result;
//
//    if (!spcspi_gen_channels(context, &channel)) {
//        return SCPI_RES_ERR;
//    }
//
//    result = rp_GenTrigger(channel);
//    if(result != RP_OK){
//        RP_LOG(LOG_ERR, "*SOUR#:TRIG:IMM Failed to set immediate "
//            "trigger: %s\n", rp_GetError(result));
//        return SCPI_RES_ERR;
//    }
//
//    RP_LOG(LOG_INFO, "*SOUR#:TRIG:IMM Successfully set immediate trigger.\n");
//    return SCPI_RES_OK;
//}
