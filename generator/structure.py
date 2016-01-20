# libsoc - Library to handle standardised output files
# Copyright (C) 2015 Rikard Nordgren
# 
# This file was autogenerated and should not be edited
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
# 
# his library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/>.

# Description of the so structure

# List of classes that need name as input to _new
need_name = [
    'Table', 'Matrix', 'ExternalFile'
]

structure = {
    'SO' : {
        'children' : [
            { 'name' : 'PharmMLRef', 'type' : 'PharmMLRef' },
            { 'name' : 'SOBlock', 'type' : 'SOBlock', 'array' : True },
        ],
        'xpath' : 'SO',
        'fields' : [ 'int error;' ]
    },
    'PharmMLRef' : {
        'attributes' : [
            'name'
        ],
        'xpath' : 'SO/PharmMLRef'
    },
    'SOBlock' : {
        'children' : [
            { 'name' : 'ToolSettings', 'type' : 'ToolSettings' },
            { 'name' : 'RawResults', 'type' : 'RawResults' },
            { 'name' : 'TaskInformation', 'type' : 'TaskInformation' },
            { 'name' : 'Estimation', 'type' : 'Estimation' },
            { 'name' : 'Simulation', 'type' : 'Simulation' },
            { 'name' : 'ModelDiagnostic', 'type' : 'ModelDiagnostic' },
            { 'name' : 'OptimalDesign', 'type' : 'OptimalDesign' },
        ],
        'attributes' : [
            'blkId'
        ],
        'xpath' : 'SO/SOBlock'
    },
    'ToolSettings' : {
        'children' : [
            { 'name' : 'File', 'type' : 'ExternalFile', 'array' : True },
        ],
        'xpath' : 'SO/SOBlock/ToolSettings'
    },
    'RawResults' : {
        'children' : [
            { 'name' : 'DataFile', 'type' : 'ExternalFile', 'array' : True },
            { 'name' : 'GraphicsFile', 'type' : 'ExternalFile', 'array' : True },
        ],
        'xpath' : 'SO/SOBlock/RawResults'
    },
    'TaskInformation' : {
        'children' : [
            { 'name' : 'Message', 'type' : 'Message', 'array' : True },
            { 'name' : 'OutputFilePath', 'type' : 'ExternalFile', 'array' : True },
            { 'name' : 'RunTime', 'type' : 'type_real' },
            { 'name' : 'NumberChains', 'type' : 'NumberChains' },
            { 'name' : 'NumberIterations', 'type' : 'NumberIterations' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation'
    },
    'Message' : {
        'children' : [
            { 'name' : 'Toolname', 'type' : 'Toolname' },
            { 'name' : 'Name', 'type' : 'Name' },
            { 'name' : 'Content', 'type' : 'Content' },
            { 'name' : 'Severity', 'type' : 'Severity' },
        ],
        'attributes' : [
            'type'
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/Message'
    },
    'NumberChains' : {
        'children' : [
            { 'name' : 'Description', 'type' : 'type_string', 'prefix' : 'ct' },
            { 'name' : 'Real', 'type' : 'type_real', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/NumberChains'
    },
    'NumberIterations' : {
        'children' : [
            { 'name' : 'Real', 'type' : 'type_real', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/NumberIterations'
    },
    'Toolname' : {
        'children' : [
            { 'name' : 'String', 'type' : 'type_string', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/Message/Toolname'
    },
    'Name' : {
        'children' : [
            { 'name' : 'String', 'type' : 'type_string', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/Message/Name'
    },
    'Content' : {
        'children' : [
            { 'name' : 'String', 'type' : 'type_string', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/Message/Content'
    },
    'Severity' : {
        'children' : [
            { 'name' : 'Int', 'type' : 'type_int', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/Message/Severity'
    },
    'Estimation' : {
        'children' : [
            { 'name' : 'PopulationEstimates', 'type' : 'PopulationEstimates' },
            { 'name' : 'PrecisionPopulationEstimates', 'type' : 'PrecisionPopulationEstimates' },
            { 'name' : 'IndividualEstimates', 'type' : 'IndividualEstimates' },
            { 'name' : 'Residuals', 'type' : 'Residuals' },
            { 'name' : 'Predictions', 'type' : 'Table' },
            { 'name' : 'Likelihood', 'type' : 'Likelihood' },
        ],
        'xpath' : 'SO/SOBlock/Estimation'
    },
    'PopulationEstimates' : {
        'children' : [
            { 'name' : 'MLE', 'type' : 'Table' },
            { 'name' : 'Bayesian', 'type' : 'Bayesian' },
            { 'name' : 'Bootstrap', 'type' : 'Bootstrap' }
        ],
        'xpath' : 'SO/SOBlock/Estimation/PopulationEstimates'
    },
    'Bootstrap' : {
        'children' : [
            { 'name' : 'Mean', 'type' : 'Table' },
            { 'name' : 'Median', 'type' : 'Table' }
        ],
        'xpath' : 'SO/SOBlock/Estimation/PopulationEstimates/Bootstrap'
    },
    'Bayesian' : {
        'children' : [
            { 'name' : 'PosteriorMean', 'type' : 'Table' },
            { 'name' : 'PosteriorMedian', 'type' : 'Table' },
            { 'name' : 'PosteriorMode', 'type' : 'Table' }
        ],
        'xpath' : 'SO/SOBlock/Estimation/PopulationEstimates/Bayesian'
    },
    'PrecisionPopulationEstimates' : {
        'children' : [
            { 'name' : 'MLE', 'type' : 'MLE' },
            { 'name' : 'Bayesian', 'type' : 'Bayesian_PPE' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/PrecisionPopulationEstimates'
    },
    'Bayesian_PPE' : {
        'element_name' : 'Bayesian',
        'children' : [
            { 'name' : 'StandardDeviationPosterior', 'type' : 'Table' },
            { 'name' : 'PercentilesCI', 'type' : 'Table' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/PrecisionPopulationEstimates/Bayesian'
    },
    'MLE' : {
        'children' : [
            { 'name' : 'FIM', 'type' : 'Matrix' },
            { 'name' : 'CovarianceMatrix', 'type' : 'Matrix' },
            { 'name' : 'CorrelationMatrix', 'type' : 'Matrix' },
            { 'name' : 'StandardError', 'type' : 'Table' },
            { 'name' : 'RelativeStandardError', 'type' : 'Table' },
            { 'name' : 'AsymptoticCI', 'type' : 'Table' },
            { 'name' : 'ConditionNumber', 'type' : 'type_real' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/PrecisionPopulationEstimates/MLE' 
    },
    'IndividualEstimates' : {
        'children' : [
            { 'name' : 'Estimates', 'type' : 'Estimates' },
            { 'name' : 'RandomEffects', 'type' : 'RandomEffects_IE' },
            { 'name' : 'EtaShrinkage', 'type' : 'Table' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/IndividualEstimates'
    },
    'Estimates' : {
        'children' : [
            { 'name' : 'Mean', 'type' : 'Table' },
            { 'name' : 'Median', 'type' : 'Table' },
            { 'name' : 'Mode', 'type' : 'Table' },
            { 'name' : 'Samples', 'type' : 'Table' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/IndividualEstimates/Estimates'
    },
    'RandomEffects_IE' : {
        'element_name' : 'RandomEffects',
        'children' : [
            { 'name' : 'EffectMean', 'type' : 'Table' },
            { 'name' : 'EffectMedian', 'type' : 'Table' },
            { 'name' : 'EffectMode', 'type' : 'Table' },
            { 'name' : 'Samples', 'type' : 'Table' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/IndividualEstimates/RandomEffects'
    },
    'Residuals' : {
        'children' : [
            { 'name' : 'ResidualTable', 'type' : 'Table' },
            { 'name' : 'EpsShrinkage', 'type' : 'Table' }
        ],
        'xpath' : 'SO/SOBlock/Estimation/Residuals'
    },
    'Likelihood' : {
        'children' : [
            { 'name' : 'LogLikelihood', 'type' : 'type_real' },
            { 'name' : 'Deviance', 'type' : 'type_real' },
            { 'name' : 'IndividualContribtoLL', 'type' : 'Table' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/Likelihood'
    },
    'Simulation' : {
        'children' : [
            { 'name' : 'SimulationBlock', 'type' : 'SimulationBlock', 'array' : True },
        ],
        'xpath' : 'SO/SOBlock/Simulation'
    },
    'SimulationBlock' : {
        'children' : [
            { 'name' : 'SimulatedProfiles', 'type' : 'SimulationSubType', 'array' : True },
            { 'name' : 'IndivParameters', 'type' : 'SimulationSubType', 'array' : True },
            { 'name' : 'RandomEffects', 'type' : 'SimulationSubType', 'array' : True },
            { 'name' : 'Covariates', 'type' : 'SimulationSubType', 'array' : True },
            { 'name' : 'PopulationParameters', 'type' : 'SimulationSubType', 'array' : True },
            { 'name' : 'Dosing', 'type' : 'SimulationSubType', 'array' : True },
            { 'name' : 'RawResultsFile', 'type' : 'ExternalFile' },
        ],
        'attributes' : [
            'replicate'
        ],
        'xpath' : 'SO/SOBlock/Simulation/SimulationBlock'
    },
    'SimulationSubType' : {
        'extends' : 'Table',
        'attributes' : [
            'name',
            'extFileNo'
        ],
        'xpath' : 'SimulationSubType'
    },
    'ExternalFile' : {
        'children' : [
            { 'name' : 'path', 'type' : 'type_string', 'prefix' : 'ds' },
            { 'name' : 'format', 'type' : 'type_string', 'prefix' : 'ds' },
            { 'name' : 'delimiter', 'type' : 'type_string', 'prefix' : 'ds' },
            { 'name' : 'MissingData', 'type' : 'MissingData', 'array' : True, 'prefix' : 'ds' }, 
            { 'name' : 'Description' , 'type' : 'type_string', 'prefix' : 'ct' },
        ],
        'attributes' : [
            'oid'
        ],
        'xpath' : 'ExternalFile'
    },
    'MissingData' : {
        'attributes' : {
            'dataCode',
            'missingDataType'
        },
        'xpath' : 'MissingData'
    },
    'OptimalDesign' : {
        'children' : [
            { 'name' : 'OptimalDesignBlock', 'type' : 'OptimalDesignBlock', 'array' : True },
        ],
        'attributes' : [
            'type'
        ],
        'xpath' : 'SO/SOBlock/OptimalDesign'
    },
    'OptimalDesignBlock' : {
        'children' : [
            { 'name' : 'FIM', 'type' : 'Matrix' },
            { 'name' : 'CovarianceMatrix', 'type' : 'Matrix' },
            { 'name' : 'ParameterPrecision', 'type' : 'Table' },
            { 'name' : 'Criteria', 'type' : 'Table' },
            { 'name' : 'Tests', 'type' : 'Table' },
            { 'name' : 'SimulatedData', 'type' : 'ExternalFile' },
            { 'name' : 'Design', 'type' : 'ExternalFile' },
        ],
        'attributes' : [
            'blockNumber'
        ],
        'xpath' : 'SO/SOBlock/OptimalDesign/OptimalDesignBlock'
    },
    'ModelDiagnostic' : {
        'children' : [
            { 'name' : 'DiagnosticPlotsStructuralModel', 'type' : 'DiagnosticPlotsStructuralModel' },
            { 'name' : 'DiagnosticPlotsIndividualParams', 'type' : 'Table' },
        ],
        'xpath' : 'SO/SOBlock/ModelDiagnostic'
    },
    'DiagnosticPlotsStructuralModel' : {
        'children' : [
            { 'name' : 'IndivFits', 'type' : 'IndivFits' },
            { 'name' : 'IndivPredictionVsObserv', 'type' : 'Table' },
            { 'name' : 'VPC', 'type' : 'Table' },
        ],
        'xpath' : 'SO/SOBlock/ModelDiagnostic/DiagnosticPlotsStructuralModel'
    },
    'IndivFits' : {
        'children' : [
            { 'name' : 'ObservationTable', 'type' : 'Table' },
            { 'name' : 'PredictionTable', 'type' : 'Table' },
        ],
        'xpath' : 'SO/SOBlock/ModelDiagnostic/DiagnosticPlotsStructuralModel/IndivFits'
    },
}
