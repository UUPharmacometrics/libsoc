#!/usr/bin/Rscript

library(methods)
library(libsoc)

od_block = so_OptimalDesignBlock$new()
od_block$blockNumber = "1"

names = c("POP_CL", "POP_V", "POP_KA", "POP_TLAG")

FIM = matrix(c(0.1,0.2,0.3,0.4), nrow=4, ncol=4, dimnames=list(names, names))
od_block$FIM = FIM

Cov = matrix(c(0.5,0.6,0.7,0.8), nrow=4, ncol=4, dimnames=list(names, names))
od_block$CovarianceMatrix = Cov

ParameterName = names
ParameterValue = c(23, 28, 29, 30)
SE = c(0.1, 0.2, 0.3, 0.4)
RSE = c(30, 40, 50, 60)
ParPrec = data.frame(ParameterName, ParameterValue, SE, RSE, stringsAsFactors=FALSE)  # The names of the columns are important here!
od_block$ParameterPrecision = ParPrec

CriteriumName = c("D-optimality")
CriteriumValue = c(371.31)
Comments = c("determinant of FIM")
Crit = data.frame(CriteriumName, CriteriumValue, Comments, stringsAsFactors=FALSE)
od_block$Criteria = Crit

ParameterName = c("beta_V_Sex_F", "beta_Cl_Treat_B", "beta_V_Sex_F")
TestName = c("Wald", "Wald", "Wald")
TestType = c("equivalence", "equivalence", "comparison")
ParameterValue = c(0.1823216, 0.09531018, 0.4054651)
LowerLimit = c(-0.2231436, -0.2231436, 0.050000)
UpperLimit = c(0.2231436, 0.2231436, 0.978421)
Type1Error = c(0.05, 0.05, 0.9)
PredictedPower = c(0.1042397, 0.9818745, 26.50458)
TargetPower = c(0.9, 0.9, 0.9)
NumberSubject = c(2286.081, 24.50351, 24.50351)
Tests = data.frame(ParameterName, TestName, TestType, ParameterValue, LowerLimit, UpperLimit,
                   Type1Error, PredictedPower, TargetPower, NumberSubject, stringsAsFactors=FALSE)
od_block$Tests = Tests

SimulatedData = so_ExternalFile$new()
SimulatedData$oid = "SimDataOid"
SimulatedData$path = "SimulatedData.dat"
od_block$SimulatedData = SimulatedData

Design = so_ExternalFile$new()
Design$oid = "designOid"
Design$path = "Design.xml"
od_block$Design = Design

od = so_OptimalDesign$new()
od$type = "evaluation"
od$add_OptimalDesignBlock(od_block)

so_block = so_SOBlock$new()
so_block$blkId = "myBlock"
so_block$OptimalDesign = od

so = so_SO$new()
so$add_SOBlock(so_block)

so_block$add_rawresults_datafile("my Description", "file", "id1")
so_block$add_message("ERROR", "PopED", "ErrorName", "This is the error description", 2)

so$write("output.SO.xml")