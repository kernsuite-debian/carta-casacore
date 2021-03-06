##################### generated by xml-casa (v2) ####################################
from __future__ import absolute_import
from CASAtools.__cerberus__ import Validator
from CASAtools.__cerberus__ import errors
import numpy
import os


class CasaValidator(Validator):
    def __is_float(self, value):
        return isinstance(value,float) or isinstance(value,numpy.float32) or isinstance(value,numpy.float64)
    def __is_floatvec(self, value):
        return isinstance(value,list) and all([self.__is_float(v) for v in value['value']])

    def __validate_anyof(self, operator, definitions, field, value):
        ### this is actually, mostly __validate_logical from the
        ### Validator class from Cerberus' validator.py
        valid_counter = 0
        _errors = errors.ErrorList()

        for i, definition in enumerate(definitions):
            schema = {field: definition.copy()}
            for rule in ('allow_unknown', 'type'):
                if rule not in schema[field] and rule in self.schema[field]:
                    schema[field][rule] = self.schema[field][rule]
            if 'allow_unknown' not in schema[field]:
                schema[field]['allow_unknown'] = self.allow_unknown

            validator = self._get_child_validator(
                schema_crumb=(field, operator, i),
                schema=schema, allow_unknown=True)

            if validator(self.document, update=self.update, normalize=True):
                valid_counter += 1
                self.document = validator.document
                break
            else:
                self._drop_nodes_from_errorpaths(validator._errors, [], [3])
                _errors.extend(validator._errors)

        return valid_counter, _errors

    def _validate_anyof(self, definitions, field, value):
        """ {'type': 'list', 'logical': 'anyof'} """
        valids, _errors = self.__validate_anyof('anyof', definitions, field, value)
        if valids < 1:
            super(CasaValidator, self)._error( field, errors.ANYOF, _errors, valids, len(definitions) )

    def _validate_type_cInt(self,value):
        if isinstance(value,int) or isinstance(value,numpy.int32) or isinstance(value,numpy.int64):
            return True

    def _validate_type_cFloat(self,value):
        if isinstance(value,float):
            return True

    def _validate_type_cStr(self,value):
        if isinstance(value,str):
            return True

    def _validate_type_cReqPath(self,value):
        if isinstance(value,str) and os.path.exists(value):
            return True

    def _validate_type_cPath(self,value):
        if isinstance(value,str):
            return True

    def _validate_type_cBool(self,value):
        if isinstance(value,bool):
            return True

    def _validate_type_cIntVec(self,value):
        if isinstance(value,list):
            if all([isinstance(v,int) or isinstance(v,numpy.int32) or isinstance(v,numpy.int64) for v in value]):
                return True
        if isinstance(value,numpy.ndarray) and len(value.shape) <= 1:
            if value.dtype.type in [ numpy.int32, numpy.int64, int ]:
                return True

    def _validate_type_cFloatVec(self,value):
        if isinstance(value,list):
            if all([isinstance(v,float) or isinstance(v,numpy.float32) or isinstance(v,numpy.float64) for v in value]):
                return True
        if isinstance(value,numpy.ndarray) and len(value.shape) <= 1:
            if value.dtype.type in [ numpy.float32, numpy.float64, float ]:
                return True

    def _validate_type_cStrVec(self,value):
        if isinstance(value,list):
            if all([isinstance(v,str) for v in value]):
                return True
        if isinstance(value,numpy.ndarray) and len(value.shape) <= 1:
            if value.dtype.type in [ numpy.str_, str ]:
                return True

    def _validate_type_cBoolVec(self,value):
        if isinstance(value,list):
            if all([isinstance(v,bool) for v in value]):
                return True
        if isinstance(value,numpy.ndarray) and len(value.shape) <= 1:
            if value.dtype.type in [ numpy.bool_, bool ]:
                return True

    def _validate_type_cIntArray(self,value):
        if isinstance(value,numpy.ndarray):
            if value.dtype.type in [int, numpy.int32, numpy.int64]:
                return True

    def _validate_type_cFloatArray(self,value):
        if isinstance(value,numpy.ndarray):
            if value.dtype.type in [ float, numpy.float32, numpy.float64 ]:
                return True

    def _validate_type_cStrArray(self,value):
        if isinstance(value,numpy.ndarray):
            if value.dtype.type is numpy.str_:
                return True

    def _validate_type_cBoolArray(self,value):
        if isinstance(value,numpy.ndarray):
            if value.dtype.type is numpy.bool_:
                return True

    def _validate_type_cDict(self,value):
        if isinstance(value,dict):
            return True

    def _validate_type_cVariant(self,value):
        if isinstance(value,str) or \
           isinstance(value,int) or \
           isinstance(value,bool) or \
           isinstance(value,dict) or \
           isinstance(value,list) or \
           isinstance(value,float) or \
           isinstance(value,numpy.ndarray):
            return True

    def _validate_type_cDoubleQuant(self,value):
        if isinstance(value,dict) and len(value) == 2 and \
           'unit' in value and 'value' in value and \
           isinstance(value['unit'],str) and \
           ( self.__is_float(value['value']) or \
             self.__is_floatvec(value['value']) ):
            return True

    def _validate_type_ccoordsysTool(self,value):
        if isinstance(value,CASAtools.coordsys.coordsys):
            return True

    def _validate_type_ccomponentlistTool(self,value):
        if isinstance(value,CASAtools.componentlist.componentlist):
            return True

    def _validate_type_cmsmetadataTool(self,value):
        if isinstance(value,CASAtools.msmetadata.msmetadata):
            return True

    def _validate_type_csynthesisimstoreTool(self,value):
        if isinstance(value,CASAtools.synthesisimstore.synthesisimstore):
            return True

    def _validate_type_cimageTool(self,value):
        if isinstance(value,CASAtools.image.image):
            return True

    def _validate_type_ctableTool(self,value):
        if isinstance(value,CASAtools.table.table):
            return True

validator = CasaValidator( )

