program main

    use libsoc
    implicit none
 
    type(so_SO) :: my_so
    type(so_SOBlock) :: block
    type(so_RawResults) :: rr
    type(so_ExternalFile) :: df
    type(so_TaskInformation) :: ti
    type(so_Estimation) :: est
    type(so_PrecisionPopulationEstimates) :: ppe
    type(so_PopulationEstimates) :: pe
    type(so_MLE) :: mle
    type(so_Matrix) :: cov
    type(so_Table) :: mle_tab
    integer :: numrows
    integer :: numdf
    integer :: i
    character,pointer,dimension(:) :: path
    real(8),pointer :: time
    real(8),dimension(:,:),pointer :: matrix
    type(c_ptr) :: column
    real(8),dimension(:),pointer :: conv_col 

    my_so = so_SO_read("../test/pheno.SO.xml")

    !if (.NOT. c_associated(my_so)) then
    !    print *, so_get_last_error()
    !    stop
    !end if

    ! Print all RawResults file paths
    block = so_SO_get_SOBlock(my_so, 0)
    rr = so_SOBlock_get_RawResults(block)
    numdf = so_RawResults_get_number_of_DataFile(rr)
!    do i = 0, numdf - 1
!        df = so_RawResults_get_DataFile(rr, i)
!        path => so_ExternalFile_get_path(df)
!        print *, path
!    end do

    ! Print RunTime
    ti = so_SOBlock_get_TaskInformation(block)
    time => so_TaskInformation_get_RunTime(ti)
    if (.NOT. associated(time)) then
        print *, "no time!"
    else
        print *, time
    end if

    ! Print rownames of covariance matrix
    est = so_SOBlock_get_Estimation(block)
    ppe = so_Estimation_get_PrecisionPopulationEstimates(est)
    mle = so_PrecisionPopulationEstimates_get_MLE(ppe)
    cov = so_MLE_get_CovarianceMatrix(mle)
    numrows = so_Matrix_get_number_of_rows(cov)

    do i = 0, numrows-1
        print *, so_Matrix_get_rownames(cov, i)
    end do

    ! Get data from matrix
    matrix => so_Matrix_get_data(cov)

    print *, matrix(1, 1)

    ! Write to matrix
    !matrix(1, 1) = 28.9

    !call so_SO_write(my_so, "changed.SO.xml", 1)

    ! Get data from table
    pe = so_Estimation_get_PopulationEstimates(est) 
    mle_tab = so_PopulationEstimates_get_mle(pe)

    column = so_Table_get_column_from_number(mle_tab, 0)
    conv_col => so_Table_data_to_real(mle_tab, column)

    print *, conv_col(1)
end
