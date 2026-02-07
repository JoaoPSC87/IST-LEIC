package hva.exceptions;

public class NoSuchResponsibilityException extends Exception {
    private final String _employeeId;
    private final String _responsibilityId;

    public NoSuchResponsibilityException(String employeeId, String responsibilityId) {
        _employeeId = employeeId;
        _responsibilityId = responsibilityId;
    }

    public String getEmployeeID() {
        return _employeeId;
    }

    public String getResponsibilityID() {
        return _responsibilityId;
    }
}