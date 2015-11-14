#!/usr/bin/python3

# Description of the so structure

# List of classes that need name as input to _new
need_name = [
    'Table', 'Matrix', 'estring'
]

structure = {
    'SO' : {
        'children' : [
            { 'name' : 'PharmMLRef', 'type' : 'PharmMLRef' },
            { 'name' : 'SOBlock', 'type' : 'SOBlock', 'array' : True },
        ],
        'xpath' : 'SO'
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
            { 'name' : 'OptimalDesign', 'type' : 'OptimalDesign' },
        ],
        'attributes' : [
            'blkId'
        ],
        'xpath' : 'SO/SOBlock'
    },
    'ToolSettings' : {
        'children' : [
            { 'name' : 'File', 'type' : 'File', 'array' : True },
        ],
        'xpath' : 'SO/SOBlock/ToolSettings'
    },
    'File' : {
        'children' : [
            { 'name' : 'Description', 'type' : 'estring', 'prefix' : 'ct' },
            { 'name' : 'path', 'type' : 'estring', 'prefix' : 'ds' },
        ],
        'attributes' : [
            'oid'
        ],
        'xpath' : 'SO/SOBlock/ToolSettings/File'
    },
    'RawResults' : {
        'children' : [
            { 'name' : 'DataFile', 'type' : 'DataFile', 'array' : True },
            { 'name' : 'GraphicsFile', 'type' : 'GraphicsFile', 'array' : True },
        ],
        'xpath' : 'SO/SOBlock/RawResults'
    },
    'DataFile' : {
        'children' : [
            { 'name' : 'Description', 'type' : 'estring', 'prefix' : 'ct' },
            { 'name' : 'path', 'type' : 'estring', 'prefix' : 'ds' },
        ],
        'attributes' : [
            'oid'
        ],
        'xpath' : 'SO/SOBlock/RawResults/DataFile'
    },
    'GraphicsFile' : {
        'children' : [
            { 'name' : 'Description', 'type' : 'estring', 'prefix' : 'ct' },
            { 'name' : 'path', 'type' : 'estring', 'prefix' : 'ds' },
        ],
        'attributes' : [
            'oid'
        ],
        'xpath' : 'SO/SOBlock/RawResults/GraphicsFile'
    },
    'TaskInformation' : {
        'children' : [
            { 'name' : 'Message', 'type' : 'Message', 'array' : True },
            { 'name' : 'OutputFilePath', 'type' : 'OutputFilePath', 'array' : True },
            { 'name' : 'RunTime', 'type' : 'RunTime' },
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
    'OutputFilePath' : {
        'children' : [
            { 'name' : 'path', 'type' : 'estring', 'prefix' : 'ds' },
        ],
        'attributes' : [
            'oid'
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/OutputFilePath'
    },
    'RunTime' : {
        'children' : [
            { 'name' : 'Description', 'type' : 'estring', 'prefix' : 'ct' },
            { 'name' : 'Real', 'type' : 'estring', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/RunTime'
    },
    'NumberChains' : {
        'children' : [
            { 'name' : 'Description', 'type' : 'estring', 'prefix' : 'ct' },
            { 'name' : 'Real', 'type' : 'estring', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/NumberChains'
    },
    'NumberIterations' : {
        'children' : [
            { 'name' : 'Real', 'type' : 'estring', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/NumberIterations'
    },
    'Toolname' : {
        'children' : [
            { 'name' : 'String', 'type' : 'estring', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/Message/Toolname'
    },
    'Name' : {
        'children' : [
            { 'name' : 'String', 'type' : 'estring', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/Message/Name'
    },
    'Content' : {
        'children' : [
            { 'name' : 'String', 'type' : 'estring', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/Message/Content'
    },
    'Severity' : {
        'children' : [
            { 'name' : 'Int', 'type' : 'estring', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/TaskInformation/Message/Severity'
    },
    'Estimation' : {
        'children' : [
            { 'name' : 'PopulationEstimates', 'type' : 'PopulationEstimates' },
            { 'name' : 'PrecisionPopulationEstimates', 'type' : 'PrecisionPopulationEstimates' },
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
        'children' : [
            { 'name' : 'StandardDeviationPosterior', 'type' : 'Table' },
            #{ 'name' : 'PosteriorDistribution', 'type' : 'PosteriorDistribution' },
            { 'name' : 'PercentilesCI', 'type' : 'Table' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/PrecisionPopulationEstimates/Bayesian'
    },
    #'PosteriorDistribution' : {
    #    'children' : [
    #        { 'name' : '
    #    ],
    #},
    'MLE' : {
        'children' : [
            { 'name' : 'FIM', 'type' : 'Matrix' },
            { 'name' : 'CovarianceMatrix', 'type' : 'Matrix' },
            { 'name' : 'CorrelationMatrix', 'type' : 'Matrix' },
            { 'name' : 'StandardError', 'type' : 'Table' },
            { 'name' : 'RelativeStandardError', 'type' : 'Table' },
            { 'name' : 'AsymptoticCI', 'type' : 'Table' },
            { 'name' : 'ConditionNumber', 'type' : 'ConditionNumber' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/PrecisionPopulationEstimates/MLE' 
    },
    'ConditionNumber' : {
        'children' : [
            { 'name' : 'Real', 'type' : 'estring', 'prefix' : 'ct' },
        ],
        'xpath' : 'SO/SOBlock/Estimation/PrecisionPopulationEstimates/ConditionNumber'
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
            { 'name' : 'LogLikelihood', 'type' : 'estring' },
            { 'name' : 'Deviance', 'type' : 'estring' },
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
            { 'name' : 'Dosing', 'type' : 'Table', 'array' : True },
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
            { 'name' : 'SimulatedData', 'type' : 'SimulatedData' },
            { 'name' : 'Design', 'type' : 'Design' },
        ],
        'attributes' : [
            'blockNumber'
        ],
        'xpath' : 'SO/SOBlock/OptimalDesign/OptimalDesignBlock'
    },
    'SimulatedData' : {
        'children' : [
            { 'name' : 'path', 'type' : 'estring', 'prefix' : 'ds' }
        ],
        'attributes' : [
            'oid'
        ],
        'xpath' : 'SO/SOBlock/OptimalDesign/OptimalDesignBlock/SimulatedData'
    },
    'Design' : {
        'children' : [
            { 'name' : 'path', 'type' : 'estring', 'prefix' : 'ds' }
        ],
        'attributes' : [
            'oid'
        ],
        'xpath' : 'SO/SOBlock/OptimalDesign/OptimalDesignBlock/Design'
    },
}

