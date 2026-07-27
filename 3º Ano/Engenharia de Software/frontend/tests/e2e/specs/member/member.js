describe('Volunteer', () => {
  const selectDateFromPicker = (inputSelector, wrapperSelector, date, referenceDate) => {
    const day = date.getDate()
    const referenceDay = referenceDate.getDate()

    // In case the day we want to select is in the next month
    if (referenceDay > day) {
      cy.get(inputSelector).click({ force: true })
      cy.get(wrapperSelector)
        .find('.datepicker-next:visible')
        .first()
        .click({ force: true })
      cy.wait(500)
    }
    cy.get(inputSelector).click({ force: true })
    cy.get(wrapperSelector)
      .find('.datepicker-day-text')
      .contains(new RegExp(`^\\s*${day}\\s*$`))
      .first()
      .click({ force: true })
  }

  beforeEach(() => {
    cy.deleteAllButArs()
    cy.createDemoEntities();
    cy.createDatabaseInfoForEnrollments()

    cy.demoMemberLogin()
  });

  afterEach(() => {
    cy.logout();
    cy.deleteAllButArs()
  });

  it('close', () => {
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="members"]').click();

    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="themes"]').click();

    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();
  });

  it('suspend activity', () => {
    const JUSTIFICATION = 'This activity will be suspended for testing.';

    cy.intercept('GET', '/users/2/getInstitution').as('activities')

    // List activities.
    cy.get('[data-cy="institution"]').click()
    cy.get('[data-cy="activities"]').click()
    cy.wait('@activities')

    // Suspend first activity.
    cy.get('[data-cy="suspendButton"]').first().click()
    cy.get('[data-cy="suspensionReasonInput"]').type(JUSTIFICATION)
    cy.get('[data-cy="suspendActivity"]').click()

    // Check if first activity is suspended.
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .should('have.length', 3)
      .should('contain', "SUSPENDED")
  });

  it('manage shifts navigation', () => {
    cy.intercept('GET', '/users/*/getInstitution').as('activities')
    cy.intercept('GET', '/activities/*/shifts').as('activityShifts')

    // Open institution activities.
    cy.get('[data-cy="institution"]').click()
    cy.get('[data-cy="activities"]').click()
    cy.wait('@activities')

    // Open activity shifts from table action.
    cy.get('[data-cy="manageShifts"]').first().click()
    cy.wait('@activityShifts')
    cy.url().should('include', '/member/shifts')
    cy.get('[data-cy="activityShiftsTable"]').should('be.visible')

    // Return to activities list.
    cy.get('[data-cy="getActivities"]').click()
    cy.url().should('include', '/member/activities')
    cy.get('[data-cy="memberActivitiesTable"]').should('be.visible')
  });

  it('create shift successfully', () => {
    const LOCATION = 'Lisbon, Portugal - New Shift';
    const PARTICIPANTS_LIMIT = '3';
    const now = new Date();
    const startDate = new Date(now);
    startDate.setDate(startDate.getDate() + 2);
    const endDate = new Date(now);
    endDate.setDate(endDate.getDate() + 3);

    cy.intercept('GET', '/users/*/getInstitution').as('activities')
    cy.intercept('GET', '/activities/*/shifts').as('activityShifts')
    cy.intercept('POST', '/activities/*/shift').as('createShift')

    // Open institution activities.
    cy.get('[data-cy="institution"]').click()
    cy.get('[data-cy="activities"]').click()
    cy.wait('@activities')

    // Open shifts page for the first activity.
    cy.get('[data-cy="manageShifts"]').first().click()
    cy.wait('@activityShifts')
    cy.url().should('include', '/member/shifts')

    // Open create shift dialog and fill mandatory fields.
    cy.get('[data-cy="createShift"]').should('not.be.disabled').click()
    
    // Fill location and participants limit
    cy.get('[data-cy="locationInput"]').type(LOCATION)
    cy.get('[data-cy="participantsLimitInput"]').type(PARTICIPANTS_LIMIT)
    
    selectDateFromPicker('#startTimeInput-input', '#startTimeInput-wrapper.date-time-picker', startDate, now)
    selectDateFromPicker('#endTimeInput-input', '#endTimeInput-wrapper.date-time-picker', endDate, now)
    cy.get('body').type('{esc}')

    // Save and validate successful creation in the shifts table.
    cy.get('[data-cy="saveShift"]').should('not.be.disabled').click()
    cy.wait('@createShift').then(({ request, response }) => {
      expect(response.statusCode).to.eq(200)
      const payloadStart = new Date(request.body.startTime)
      const payloadEnd = new Date(request.body.endTime)
      expect(payloadStart.getTime()).to.be.lessThan(payloadEnd.getTime())
    })
    cy.get('[data-cy="activityShiftsTable"] tbody tr').should('contain', LOCATION)
  });
});
